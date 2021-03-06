#include "AHX.h"
#if defined(__AVX__)
#	include "KdfFromName.h"
#	include "IntUtils.h"
#	include "MemUtils.h"
#	include "UInt128.h"
#endif

NAMESPACE_BLOCK

#if defined(__AVX__)

const std::string AHX::CIPHER_NAME("Rijndael");
const std::string AHX::CLASS_NAME("AHX");
const std::string AHX::DEF_DSTINFO("information string RHX version 1");

//~~~Constructor~~~//
AHX::AHX(BlockCipherExtensions CipherExtension)
	:
	m_cprExtension(CipherExtension),
	m_destroyEngine(true),
	m_distCode(DEF_DSTINFO.begin(), DEF_DSTINFO.end()),
	m_distCodeMax(0),
	m_expKey(0),
	m_kdfGenerator(CipherExtension == BlockCipherExtensions::None ? nullptr :
		CipherExtension == BlockCipherExtensions::Custom ? throw CryptoSymmetricCipherException("AHX:CTor", "The Kdf can not be null!") :
		Helper::KdfFromName::GetInstance(static_cast<Enumeration::Kdfs>(CipherExtension))),
	m_isDestroyed(false),
	m_isEncryption(false),
	m_isInitialized(false),
	m_legalKeySizes(0)
{
	LoadState();
}

AHX::AHX(Kdf::IKdf* Kdf)
	:
	m_cprExtension(BlockCipherExtensions::Custom),
	m_destroyEngine(false),
	m_distCode(DEF_DSTINFO.begin(), DEF_DSTINFO.end()),
	m_distCodeMax(0),
	m_expKey(0),
	m_kdfGenerator(Kdf != nullptr ? Kdf :
		throw CryptoSymmetricCipherException("AHX:CTor", "The Kdf can not be null!")),
	m_isDestroyed(false),
	m_isEncryption(false),
	m_isInitialized(false),
	m_legalKeySizes(0)
{
	LoadState();
}

AHX::~AHX()
{
	if (!m_isDestroyed)
	{
		m_isDestroyed = true;
		m_distCodeMax = 0;
		m_isEncryption = false;
		m_isInitialized = false;
		m_rndCount = 0;

		Utility::IntUtils::ClearVector(m_expKey);
		Utility::IntUtils::ClearVector(m_distCode);
		Utility::IntUtils::ClearVector(m_legalKeySizes);

		if (m_destroyEngine)
		{
			m_destroyEngine = false;

			if (m_kdfGenerator != nullptr)
			{
				m_kdfGenerator.reset(nullptr);
			}
		}
		else
		{
			if (m_kdfGenerator != nullptr)
			{
				m_kdfGenerator.release();
			}
		}
	}
}

//~~~Accessors~~~//

const size_t AHX::BlockSize()
{
	return BLOCK_SIZE;
}

const BlockCipherExtensions AHX::CipherExtension()
{
	return m_cprExtension;
}

std::vector<byte> &AHX::DistributionCode()
{
	return m_distCode;
}

const size_t AHX::DistributionCodeMax()
{
	return m_distCodeMax;
}

const BlockCiphers AHX::Enumeral()
{
	return (m_cprExtension == BlockCipherExtensions::None) ? BlockCiphers::Rijndael : BlockCiphers::AHX;
}

const bool AHX::IsEncryption()
{
	return m_isEncryption;
}

const bool AHX::IsInitialized()
{
	return m_isInitialized;
}

const std::vector<SymmetricKeySize> &AHX::LegalKeySizes()
{
	return m_legalKeySizes;
}

const std::string AHX::Name()
{
	std::string txtName = "";

	if (m_cprExtension == BlockCipherExtensions::SHAKE256)
	{
		txtName = CIPHER_NAME + std::string("-SHAKE-256");
	}
	else if (m_cprExtension == BlockCipherExtensions::SHAKE512)
	{
		txtName = CLASS_NAME + std::string("-SHAKE512-");
	}
	else if (m_cprExtension == BlockCipherExtensions::HKDF256)
	{
		txtName = CLASS_NAME + std::string("-HKDF-SHA2-256");
	}
	else if (m_cprExtension == BlockCipherExtensions::HKDF512)
	{
		txtName = CLASS_NAME + std::string("-HKDF-SHA2-512");
	}
	else
	{
		txtName = CIPHER_NAME;
	}

	return txtName;
}

const size_t AHX::Rounds()
{
	return m_rndCount;
}

const size_t AHX::StateCacheSize()
{
	return STATE_PRECACHED;
}

//~~~Public Functions~~~//

void AHX::DecryptBlock(const std::vector<byte> &Input, std::vector<byte> &Output)
{
	Decrypt128(Input, 0, Output, 0);
}

void AHX::DecryptBlock(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset)
{
	Decrypt128(Input, InOffset, Output, OutOffset);
}

void AHX::EncryptBlock(const std::vector<byte> &Input, std::vector<byte> &Output)
{
	Encrypt128(Input, 0, Output, 0);
}

void AHX::EncryptBlock(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset)
{
	Encrypt128(Input, InOffset, Output, OutOffset);
}

void AHX::Initialize(bool Encryption, ISymmetricKey &KeyParams)
{
	if (!SymmetricKeySize::Contains(m_legalKeySizes, KeyParams.Key().size()))
	{
		throw CryptoSymmetricCipherException("AHX:Initialize", "Invalid key size! Key must be one of the LegalKeySizes() in length.");
	}
	if (m_cprExtension != BlockCipherExtensions::None && KeyParams.Info().size() > m_distCodeMax)
	{
		throw CryptoSymmetricCipherException("AHX:Initialize", "Invalid info size! Info parameter must be no longer than DistributionCodeMax size.");
	}

	if (KeyParams.Info().size() > 0)
	{
		m_distCode = KeyParams.Info();
	}

	m_isEncryption = Encryption;
	// expand the key
	ExpandKey(Encryption, KeyParams.Key());
	// ready to transform data
	m_isInitialized = true;
}

void AHX::Transform(const std::vector<byte> &Input, std::vector<byte> &Output)
{
	if (m_isEncryption)
	{
		Encrypt128(Input, 0, Output, 0);
	}
	else
	{
		Decrypt128(Input, 0, Output, 0);
	}
}

void AHX::Transform(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset)
{
	if (m_isEncryption)
	{
		Encrypt128(Input, InOffset, Output, OutOffset);
	}
	else
	{
		Decrypt128(Input, InOffset, Output, OutOffset);
	}
}

void AHX::Transform512(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset)
{
	if (m_isEncryption)
	{
		Encrypt512(Input, InOffset, Output, OutOffset);
	}
	else
	{
		Decrypt512(Input, InOffset, Output, OutOffset);
	}
}

void AHX::Transform1024(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset)
{
	if (m_isEncryption)
	{
		Encrypt1024(Input, InOffset, Output, OutOffset);
	}
	else
	{
		Decrypt1024(Input, InOffset, Output, OutOffset);
	}
}

void AHX::Transform2048(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset)
{
	if (m_isEncryption)
	{
		Encrypt2048(Input, InOffset, Output, OutOffset);
	}
	else
	{
		Decrypt2048(Input, InOffset, Output, OutOffset);
	}
}

//~~~Key Schedule~~~//

void AHX::ExpandKey(bool Encryption, const std::vector<byte> &Key)
{
	if (m_cprExtension != BlockCipherExtensions::None)
	{
		// kdf key expansion
		SecureExpand(Key);
	}
	else
	{
		// standard rijndael key expansion + k512
		StandardExpand(Key);
	}

	// inverse cipher
	if (!Encryption)
	{
		size_t i;
		size_t j;

		std::swap(m_expKey[0], m_expKey[m_expKey.size() - 1]);

		for (i = 1, j = m_expKey.size() - 2; i < j; ++i, --j)
		{
			__m128i temp = _mm_aesimc_si128(m_expKey[i]);
			m_expKey[i] = _mm_aesimc_si128(m_expKey[j]);
			m_expKey[j] = temp;
		}

		m_expKey[i] = _mm_aesimc_si128(m_expKey[i]);
	}
}

void AHX::SecureExpand(const std::vector<byte> &Key)
{
	// rounds: k256=22, k512=30, k1024=38
	m_rndCount = Key.size() != 128 ? (Key.size() / 4) + 14 : 38;
	// round-key array size
	size_t keySize = ((BLOCK_SIZE / sizeof(uint)) * (m_rndCount + 1)) / 4;
	std::vector<byte> rawKey(keySize * sizeof(__m128i));
	// salt is not used
	std::vector<byte> salt(0);
	// initialize the generator
	m_kdfGenerator->Initialize(Key, salt, m_distCode);
	// generate the keying material
	m_kdfGenerator->Generate(rawKey);
	// initialize round-key array
	m_expKey.resize(keySize);

	// initialize working key
	m_expKey.resize(keySize);

	// big endian format to align with test vectors
	for (size_t i = 0; i < rawKey.size(); i += 4)
	{
		uint tmpbk = Utility::IntUtils::BeBytesTo32(rawKey, i);
		Utility::IntUtils::Le32ToBytes(tmpbk, rawKey, i);
	}

	// copy bytes to working key
	for (size_t i = 0, j = 0; i < keySize; ++i, j += 16)
	{
		m_expKey[i] = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&rawKey[j]));
	}
}

void AHX::StandardExpand(const std::vector<byte> &Key)
{
	// block in 32 bit words
	size_t blkWords = BLOCK_SIZE / sizeof(uint);
	// key in 32 bit words
	size_t keyWords = Key.size() / sizeof(uint);;
	// rounds calculation, 512 gets 22 rounds
	m_rndCount = keyWords + 6;
	// setup expanded key
	m_expKey.resize((blkWords * (m_rndCount + 1)) / sizeof(uint));

	if (keyWords == 16)
	{
		m_expKey[0] = _mm_loadu_si128(reinterpret_cast<const __m128i*>(Key.data()));
		m_expKey[1] = _mm_loadu_si128(reinterpret_cast<const __m128i*>(Key.data() + 16));
		m_expKey[2] = _mm_loadu_si128(reinterpret_cast<const __m128i*>(Key.data() + 32));
		m_expKey[3] = _mm_loadu_si128(reinterpret_cast<const __m128i*>(Key.data() + 48));
		m_expKey[4] = _mm_aeskeygenassist_si128(m_expKey[3], 0x01);
		ExpandRotBlock(m_expKey, 4, 4);
		ExpandSubBlock(m_expKey, 5, 4);
		m_expKey[6] = _mm_aeskeygenassist_si128(m_expKey[5], 0x02);
		ExpandRotBlock(m_expKey, 6, 4);
		ExpandSubBlock(m_expKey, 7, 4);
		m_expKey[8] = _mm_aeskeygenassist_si128(m_expKey[7], 0x04);
		ExpandRotBlock(m_expKey, 8, 4);
		ExpandSubBlock(m_expKey, 9, 4);
		m_expKey[10] = _mm_aeskeygenassist_si128(m_expKey[9], 0x08);
		ExpandRotBlock(m_expKey, 10, 4);
		ExpandSubBlock(m_expKey, 11, 4);
		m_expKey[12] = _mm_aeskeygenassist_si128(m_expKey[11], 0x10);
		ExpandRotBlock(m_expKey, 12, 4);
		ExpandSubBlock(m_expKey, 13, 4);
		m_expKey[14] = _mm_aeskeygenassist_si128(m_expKey[13], 0x20);
		ExpandRotBlock(m_expKey, 14, 4);
		ExpandSubBlock(m_expKey, 15, 4);
		m_expKey[16] = _mm_aeskeygenassist_si128(m_expKey[15], 0x40);
		ExpandRotBlock(m_expKey, 16, 4);
		ExpandSubBlock(m_expKey, 17, 4);
		m_expKey[18] = _mm_aeskeygenassist_si128(m_expKey[17], 0x80);
		ExpandRotBlock(m_expKey, 18, 4);
		ExpandSubBlock(m_expKey, 19, 4);
		m_expKey[20] = _mm_aeskeygenassist_si128(m_expKey[19], 0x1B);
		ExpandRotBlock(m_expKey, 20, 4);
		ExpandSubBlock(m_expKey, 21, 4);
		m_expKey[22] = _mm_aeskeygenassist_si128(m_expKey[21], 0x36);
		ExpandRotBlock(m_expKey, 22, 4);
	}
	else if (keyWords == 8)
	{
		m_expKey[0] = _mm_loadu_si128(reinterpret_cast<const __m128i*>(Key.data()));
		m_expKey[1] = _mm_loadu_si128(reinterpret_cast<const __m128i*>(Key.data() + 16));
		m_expKey[2] = _mm_aeskeygenassist_si128(m_expKey[1], 0x01);
		ExpandRotBlock(m_expKey, 2, 2);
		ExpandSubBlock(m_expKey, 3, 2);
		m_expKey[4] = _mm_aeskeygenassist_si128(m_expKey[3], 0x02);
		ExpandRotBlock(m_expKey, 4, 2);
		ExpandSubBlock(m_expKey, 5, 2);
		m_expKey[6] = _mm_aeskeygenassist_si128(m_expKey[5], 0x04);
		ExpandRotBlock(m_expKey, 6, 2);
		ExpandSubBlock(m_expKey, 7, 2);
		m_expKey[8] = _mm_aeskeygenassist_si128(m_expKey[7], 0x08);
		ExpandRotBlock(m_expKey, 8, 2);
		ExpandSubBlock(m_expKey, 9, 2);
		m_expKey[10] = _mm_aeskeygenassist_si128(m_expKey[9], 0x10);
		ExpandRotBlock(m_expKey, 10, 2);
		ExpandSubBlock(m_expKey, 11, 2);
		m_expKey[12] = _mm_aeskeygenassist_si128(m_expKey[11], 0x20);
		ExpandRotBlock(m_expKey, 12, 2);
		ExpandSubBlock(m_expKey, 13, 2);
		m_expKey[14] = _mm_aeskeygenassist_si128(m_expKey[13], 0x40);
		ExpandRotBlock(m_expKey, 14, 2);
	}
	else if (keyWords == 6)
	{
		__m128i K0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(Key.data()));
		__m128i K1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(Key.data() + 8));

		K1 = _mm_srli_si128(K1, 8);
		m_expKey[0] = K0;
		m_expKey[1] = K1;
		ExpandRotBlock(m_expKey, &K0, &K1, _mm_aeskeygenassist_si128(K1, 0x01), 24);
		ExpandRotBlock(m_expKey, &K0, &K1, _mm_aeskeygenassist_si128(K1, 0x02), 48);
		ExpandRotBlock(m_expKey, &K0, &K1, _mm_aeskeygenassist_si128(K1, 0x04), 72);
		ExpandRotBlock(m_expKey, &K0, &K1, _mm_aeskeygenassist_si128(K1, 0x08), 96);
		ExpandRotBlock(m_expKey, &K0, &K1, _mm_aeskeygenassist_si128(K1, 0x10), 120);
		ExpandRotBlock(m_expKey, &K0, &K1, _mm_aeskeygenassist_si128(K1, 0x20), 144);
		ExpandRotBlock(m_expKey, &K0, &K1, _mm_aeskeygenassist_si128(K1, 0x40), 168);
		ExpandRotBlock(m_expKey, &K0, &K1, _mm_aeskeygenassist_si128(K1, 0x80), 192);
	}
	else
	{
		m_expKey[0] = _mm_loadu_si128(reinterpret_cast<const __m128i*>(Key.data()));
		m_expKey[1] = _mm_aeskeygenassist_si128(m_expKey[0], 0x01);
		ExpandRotBlock(m_expKey, 1, 1);
		m_expKey[2] = _mm_aeskeygenassist_si128(m_expKey[1], 0x02);
		ExpandRotBlock(m_expKey, 2, 1);
		m_expKey[3] = _mm_aeskeygenassist_si128(m_expKey[2], 0x04);
		ExpandRotBlock(m_expKey, 3, 1);
		m_expKey[4] = _mm_aeskeygenassist_si128(m_expKey[3], 0x08);
		ExpandRotBlock(m_expKey, 4, 1);
		m_expKey[5] = _mm_aeskeygenassist_si128(m_expKey[4], 0x10);
		ExpandRotBlock(m_expKey, 5, 1);
		m_expKey[6] = _mm_aeskeygenassist_si128(m_expKey[5], 0x20);
		ExpandRotBlock(m_expKey, 6, 1);
		m_expKey[7] = _mm_aeskeygenassist_si128(m_expKey[6], 0x40);
		ExpandRotBlock(m_expKey, 7, 1);
		m_expKey[8] = _mm_aeskeygenassist_si128(m_expKey[7], 0x80);
		ExpandRotBlock(m_expKey, 8, 1);
		m_expKey[9] = _mm_aeskeygenassist_si128(m_expKey[8], 0x1B);
		ExpandRotBlock(m_expKey, 9, 1);
		m_expKey[10] = _mm_aeskeygenassist_si128(m_expKey[9], 0x36);
		ExpandRotBlock(m_expKey, 10, 1);
	}
}

void AHX::ExpandRotBlock(std::vector<__m128i> &Key, __m128i* K1, __m128i* K2, __m128i KR, size_t Offset)
{
	// 192 bit key expansion method, -requires additional processing
	__m128i key1 = *K1; 
	__m128i key2 = *K2;

	KR = _mm_shuffle_epi32(KR, _MM_SHUFFLE(1, 1, 1, 1));
	key1 = _mm_xor_si128(key1, _mm_slli_si128(key1, 4));
	key1 = _mm_xor_si128(key1, _mm_slli_si128(key1, 4));
	key1 = _mm_xor_si128(key1, _mm_slli_si128(key1, 4));
	key1 = _mm_xor_si128(key1, KR);
	*K1 = key1;

	std::memcpy(((byte*)Key.data() + Offset), &key1, 16);

	if (!(Offset == 192 && Key.size() == 13))
	{
		key2 = _mm_xor_si128(key2, _mm_slli_si128(key2, 4));
		key2 = _mm_xor_si128(key2, _mm_shuffle_epi32(key1, _MM_SHUFFLE(3, 3, 3, 3)));
		*K2 = key2;

		Offset += 16;
		std::vector<byte> tmpB(4);
		Utility::IntUtils::Le32ToBytes(_mm_cvtsi128_si32(key2), tmpB, 0);
		std::memcpy((byte*)Key.data() + Offset, &tmpB[0], 4);

		Offset += 4;
		Utility::IntUtils::Le32ToBytes(_mm_cvtsi128_si32(_mm_srli_si128(key2, 4)), tmpB, 0);
		std::memcpy((byte*)Key.data() + Offset, &tmpB[0], 4);
	}
}

void AHX::ExpandRotBlock(std::vector<__m128i> &Key, const size_t Index, const size_t Offset)
{
	// 128, 256, 512 bit key method
	__m128i pkb = Key[Index - Offset];
	Key[Index] = _mm_shuffle_epi32(Key[Index], 0xFF);
	pkb = _mm_xor_si128(pkb, _mm_slli_si128(pkb, 0x4));
	pkb = _mm_xor_si128(pkb, _mm_slli_si128(pkb, 0x4));
	pkb = _mm_xor_si128(pkb, _mm_slli_si128(pkb, 0x4));
	Key[Index] = _mm_xor_si128(pkb, Key[Index]);
}

void AHX::ExpandSubBlock(std::vector<__m128i> &Key, const size_t Index, const size_t Offset)
{
	// used with 256 and 512 bit keys
	__m128i pkb = Key[Index - Offset];
	Key[Index] = _mm_shuffle_epi32(_mm_aeskeygenassist_si128(Key[Index - 1], 0x0), 0xAA);
	pkb = _mm_xor_si128(pkb, _mm_slli_si128(pkb, 0x4));
	pkb = _mm_xor_si128(pkb, _mm_slli_si128(pkb, 0x4));
	pkb = _mm_xor_si128(pkb, _mm_slli_si128(pkb, 0x4));
	Key[Index] = _mm_xor_si128(pkb, Key[Index]);
}

//~~~Rounds Processing~~~//

void AHX::Decrypt128(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset)
{
	const size_t RNDCNT = m_expKey.size() - 2;
	size_t keyCtr = 0;

	__m128i X = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&Input[InOffset]));
	X = _mm_xor_si128(X, m_expKey[keyCtr]);

	while (keyCtr != RNDCNT)
	{
		++keyCtr;
		X = _mm_aesdec_si128(X, m_expKey[keyCtr]);
	}

	++keyCtr;
	_mm_storeu_si128(reinterpret_cast<__m128i*>(&Output[OutOffset]), _mm_aesdeclast_si128(X, m_expKey[keyCtr]));
}

void AHX::Decrypt512(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset)
{
	const size_t RNDCNT = m_expKey.size() - 2;
	size_t keyCtr = 0;

	Numeric::UInt128 X0(Input, InOffset);
	Numeric::UInt128 X1(Input, InOffset + 16);
	Numeric::UInt128 X2(Input, InOffset + 32);
	Numeric::UInt128 X3(Input, InOffset + 48);

	X0.xmm = _mm_xor_si128(X0.xmm, m_expKey[keyCtr]);
	X1.xmm = _mm_xor_si128(X1.xmm, m_expKey[keyCtr]);
	X2.xmm = _mm_xor_si128(X2.xmm, m_expKey[keyCtr]);
	X3.xmm = _mm_xor_si128(X3.xmm, m_expKey[keyCtr]);

	while (keyCtr != RNDCNT)
	{
		++keyCtr;
		X0.xmm = _mm_aesdec_si128(X0.xmm, m_expKey[keyCtr]);
		X1.xmm = _mm_aesdec_si128(X1.xmm, m_expKey[keyCtr]);
		X2.xmm = _mm_aesdec_si128(X2.xmm, m_expKey[keyCtr]);
		X3.xmm = _mm_aesdec_si128(X3.xmm, m_expKey[keyCtr]);
	}

	++keyCtr;
	X0.xmm = _mm_aesdeclast_si128(X0.xmm, m_expKey[keyCtr]);
	X1.xmm = _mm_aesdeclast_si128(X1.xmm, m_expKey[keyCtr]);
	X2.xmm = _mm_aesdeclast_si128(X2.xmm, m_expKey[keyCtr]);
	X3.xmm = _mm_aesdeclast_si128(X3.xmm, m_expKey[keyCtr]);

	X0.Store(Output, OutOffset);
	X1.Store(Output, OutOffset + 16);
	X2.Store(Output, OutOffset + 32);
	X3.Store(Output, OutOffset + 48);
}

void AHX::Decrypt1024(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset)
{
	// no aes-ni 256 api.. yet
	Decrypt512(Input, InOffset, Output, OutOffset);
	Decrypt512(Input, InOffset + 64, Output, OutOffset + 64);
}

void AHX::Decrypt2048(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset)
{
	Decrypt1024(Input, InOffset, Output, OutOffset);
	Decrypt1024(Input, InOffset + 128, Output, OutOffset + 128);
}

void AHX::Encrypt128(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset)
{
	const size_t RNDCNT = m_expKey.size() - 2;
	size_t keyCtr = 0;

	__m128i X = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&Input[InOffset]));
	X = _mm_xor_si128(X, m_expKey[keyCtr]);

	while (keyCtr != RNDCNT)
	{
		++keyCtr;
		X = _mm_aesenc_si128(X, m_expKey[keyCtr]);
	}

	++keyCtr;
	_mm_storeu_si128(reinterpret_cast<__m128i*>(&Output[OutOffset]), _mm_aesenclast_si128(X, m_expKey[keyCtr]));
}

void AHX::Encrypt512(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset)
{
	const size_t RNDCNT = m_expKey.size() - 2;
	size_t keyCtr = 0;

	Numeric::UInt128 X0(Input, InOffset);
	Numeric::UInt128 X1(Input, InOffset + 16);
	Numeric::UInt128 X2(Input, InOffset + 32);
	Numeric::UInt128 X3(Input, InOffset + 48);

	X0.xmm = _mm_xor_si128(X0.xmm, m_expKey[keyCtr]);
	X1.xmm = _mm_xor_si128(X1.xmm, m_expKey[keyCtr]);
	X2.xmm = _mm_xor_si128(X2.xmm, m_expKey[keyCtr]);
	X3.xmm = _mm_xor_si128(X3.xmm, m_expKey[keyCtr]);

	while (keyCtr != RNDCNT)
	{
		++keyCtr;
		X0.xmm = _mm_aesenc_si128(X0.xmm, m_expKey[keyCtr]);
		X1.xmm = _mm_aesenc_si128(X1.xmm, m_expKey[keyCtr]);
		X2.xmm = _mm_aesenc_si128(X2.xmm, m_expKey[keyCtr]);
		X3.xmm = _mm_aesenc_si128(X3.xmm, m_expKey[keyCtr]);
	}

	++keyCtr;
	X0.xmm = _mm_aesenclast_si128(X0.xmm, m_expKey[keyCtr]);
	X1.xmm = _mm_aesenclast_si128(X1.xmm, m_expKey[keyCtr]);
	X2.xmm = _mm_aesenclast_si128(X2.xmm, m_expKey[keyCtr]);
	X3.xmm = _mm_aesenclast_si128(X3.xmm, m_expKey[keyCtr]);

	X0.Store(Output, OutOffset);
	X1.Store(Output, OutOffset + 16);
	X2.Store(Output, OutOffset + 32);
	X3.Store(Output, OutOffset + 48);
}

void AHX::Encrypt1024(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset)
{
	Encrypt512(Input, InOffset, Output, OutOffset);
	Encrypt512(Input, InOffset + 64, Output, OutOffset + 64);
}

void AHX::Encrypt2048(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset)
{
	Encrypt1024(Input, InOffset, Output, OutOffset);
	Encrypt1024(Input, InOffset + 128, Output, OutOffset + 128);
}

//~~~Helpers~~~//

void AHX::LoadState()
{
	if (m_cprExtension == BlockCipherExtensions::None)
	{
		m_legalKeySizes.resize(4);
		m_legalKeySizes[0] = SymmetricKeySize(16, BLOCK_SIZE, 0);
		m_legalKeySizes[1] = SymmetricKeySize(24, BLOCK_SIZE, 0);
		m_legalKeySizes[2] = SymmetricKeySize(32, BLOCK_SIZE, 0);
		m_legalKeySizes[3] = SymmetricKeySize(64, BLOCK_SIZE, 0);
	}
	else
	{
		m_legalKeySizes.resize(3);

		if (m_cprExtension == BlockCipherExtensions::SHAKE256)
		{
			// sha3-256 blocksize
			m_distCodeMax = 136;
		}
		else if (m_cprExtension == BlockCipherExtensions::SHAKE512)
		{
			// sha3-512 blocksize
			m_distCodeMax = 72;
		}
		else if (m_cprExtension == BlockCipherExtensions::HKDF512)
		{
			// hmac(sha2-512) mac size
			m_distCodeMax = 64;
		}
		else
		{
			// hmac(sha2-256) mac size
			m_distCodeMax = 32;
		}

		m_legalKeySizes[0] = SymmetricKeySize(32, BLOCK_SIZE, m_distCodeMax);
		m_legalKeySizes[1] = SymmetricKeySize(64, BLOCK_SIZE, m_distCodeMax);
		m_legalKeySizes[2] = SymmetricKeySize(128, BLOCK_SIZE, m_distCodeMax);
	}
}

#endif
NAMESPACE_BLOCKEND

