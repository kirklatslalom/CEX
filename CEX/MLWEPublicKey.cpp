#include "MLWEPublicKey.h"
#include "IntUtils.h"

NAMESPACE_ASYMMETRICKEY

//~~~Constructor~~~//

MLWEPublicKey::MLWEPublicKey(MLWEParams Parameters, std::vector<byte> &P)
	:
	m_isDestroyed(false),
	m_rlweParameters(Parameters),
	m_pCoeffs(P)
{
}

MLWEPublicKey::MLWEPublicKey(const std::vector<byte> &KeyStream)
	:
	m_isDestroyed(false),
	m_rlweParameters(MLWEParams::None),
	m_pCoeffs(0)
{
	m_rlweParameters = static_cast<MLWEParams>(KeyStream[0]);
	uint pLen = Utility::IntUtils::LeBytesTo32(KeyStream, 1);
	m_pCoeffs.resize(pLen);
	Utility::MemUtils::Copy(KeyStream, 5, m_pCoeffs, 0, pLen);
}

MLWEPublicKey::~MLWEPublicKey()
{
	Destroy();
}

//~~~Accessors~~~//

const AsymmetricEngines MLWEPublicKey::CipherType()
{
	return Enumeration::AsymmetricEngines::ModuleLWE;
}

const AsymmetricKeyTypes MLWEPublicKey::KeyType()
{
	return AsymmetricKeyTypes::CipherPublicKey;
}

const MLWEParams MLWEPublicKey::Parameters()
{
	return m_rlweParameters;
}

const std::vector<byte> &MLWEPublicKey::P()
{
	return m_pCoeffs;
}

//~~~Public Functions~~~//

void MLWEPublicKey::Destroy()
{
	if (!m_isDestroyed)
	{
		m_isDestroyed = true;
		m_rlweParameters = MLWEParams::None;

		if (m_pCoeffs.size() > 0)
		{
			Utility::IntUtils::ClearVector(m_pCoeffs);
		}
	}
}

std::vector<byte> MLWEPublicKey::ToBytes()
{
	uint pLen = static_cast<uint>(m_pCoeffs.size());
	std::vector<byte> p(pLen + 5);

	p[0] = static_cast<byte>(m_rlweParameters);
	Utility::IntUtils::Le32ToBytes(pLen, p, 1);
	Utility::MemUtils::Copy(m_pCoeffs, 0, p, 5, pLen);

	return p;
}

NAMESPACE_ASYMMETRICKEYEND
