#include "KeccakTest.h"
#include "HMAC.h"
#include "Keccak256.h"
#include "Keccak512.h"
#include "KeyParams.h"

namespace Test
{
	std::string KeccakTest::Run()
	{
		try
		{
			Initialize();

			CEX::Digest::Keccak256* kc224 = new CEX::Digest::Keccak256(224);
			CEX::Digest::Keccak256* kc256 = new CEX::Digest::Keccak256(256);
			CEX::Digest::Keccak256* kc288 = new CEX::Digest::Keccak256(288);
			CEX::Digest::Keccak512* kc384 = new CEX::Digest::Keccak512(384);
			CEX::Digest::Keccak512* kc512 = new CEX::Digest::Keccak512(512);

			CompareVector(kc224, m_expected224);
			OnProgress("Passed Keccak 224 bit digest vector tests..");
			CompareVector(kc256, m_expected256);
			OnProgress("Passed Keccak 256 bit digest vector tests..");
			CompareVector(kc288, m_expected288);
			OnProgress("Passed Keccak 288 bit digest vector tests..");
			CompareVector(kc384, m_expected384);
			OnProgress("KeccakTest: Passed Keccak 384 bit digest vector tests..");
			CompareVector(kc512, m_expected512);
			OnProgress("KeccakTest: Passed Keccak 512 bit digest vector tests..");

			CompareHMAC(kc224, m_mac224, m_trunc224);
			OnProgress("Passed Keccak 224 bit digest HMAC tests..");
			CompareHMAC(kc256, m_mac256, m_trunc256);
			OnProgress("Passed Keccak 256 bit digest HMAC tests..");
			CompareHMAC(kc384, m_mac384, m_trunc384);
			OnProgress("KeccakTest: Passed Keccak 384 bit digest HMAC tests..");
			CompareHMAC(kc512, m_mac512, m_trunc512);
			OnProgress("KeccakTest: Passed Keccak 512 bit digest HMAC tests..");

			delete kc224;
			delete kc256;
			delete kc288;
			delete kc384;
			delete kc512;

			return SUCCESS;
		}
		catch (std::string const& ex)
		{
			throw TestException(std::string(FAILURE + " : " + ex));
		}
		catch (...)
		{
			throw TestException(std::string(FAILURE + " : Internal Error"));
		}
	}

	void KeccakTest::CompareVector(CEX::Digest::IDigest* Digest, std::vector<std::vector<byte>> &Expected)
	{
		std::vector<byte> hash(Digest->DigestSize(), 0);

		for (unsigned int i = 0; i != m_messages.size(); i++)
		{
			if (m_messages[i].size() != 0)
				Digest->BlockUpdate(m_messages[i], 0, m_messages[i].size());

			Digest->DoFinal(hash, 0);

			if (Expected[i] != hash)
				throw std::string("Keccak: Expected hash is not equal!");
		}

		std::vector<byte> k64(1024 * 64, 0);

		for (unsigned int i = 0; i != k64.size(); i++)
			k64[i] = (byte)'a';

		Digest->BlockUpdate(k64, 0, k64.size());
		Digest->DoFinal(hash, 0);

		if (Expected[m_messages.size()] != hash)
			throw std::string("Keccak: Expected hash is not equal!");

		for (unsigned int i = 0; i != k64.size(); i++)
			Digest->Update((byte)'a');

		Digest->DoFinal(hash, 0);

		if (Expected[m_messages.size()] != hash)
			throw std::string("Keccak: Expected hash is not equal!");

		for (unsigned int i = 0; i != k64.size(); i++)
			k64[i] = (byte)('a' + (i % 26));

		Digest->BlockUpdate(k64, 0, k64.size());
		Digest->DoFinal(hash, 0);

		if (Expected[m_messages.size() + 1] != hash)
			throw std::string("Keccak: Expected hash is not equal!");

		for (unsigned int i = 0; i != 64; i++)
		{
			Digest->Update(k64[i * 1024]);
			Digest->BlockUpdate(k64, i * 1024 + 1, 1023);
		}

		Digest->DoFinal(hash, 0);

		if (Expected[m_messages.size() + 1] != hash)
			throw std::string("Keccak: Expected hash is not equal!");

		CompareDoFinal(Digest);

		/*// very long test (it passes)
		for (unsigned int i = 0; i != 16384; i++)
		{
		for (int j = 0; j != 1024; j++)
		Digest->BlockUpdate(m_xtremeData, 0, m_xtremeData.size());
		}

		Digest->DoFinal(hash, 0);

		if ((Expected[m_messages.size() + 2]) != hash)
		throw std::string("Keccak: Expected hash is not equal!");*/
	}

	void KeccakTest::CompareDoFinal(CEX::Digest::IDigest* Digest)
	{
		std::vector<byte> hash(Digest->DigestSize(), 0);

		Digest->DoFinal(hash, 0);

		for (unsigned int i = 0; i <= Digest->DigestSize(); ++i)
		{
			std::vector<byte> expected(2 * Digest->DigestSize(), 0);
			memcpy(&expected[i], &hash[0], hash.size());
			std::vector<byte> outBytes(2 * Digest->DigestSize(), 0);

			Digest->DoFinal(outBytes, i);

			if (expected != outBytes)
				throw std::string("Keccak DoFinal: Expected hash is not equal!");
		}
	}

	void KeccakTest::CompareHMAC(CEX::Digest::IDigest* Digest, std::vector<std::vector<byte>> &Expected, std::vector<byte> &TruncExpected)
	{
		CEX::Mac::HMAC mac(Digest);
		std::vector<byte> macV2(mac.MacSize(), 0);

		for (unsigned int i = 0; i != m_macKeys.size(); i++)
		{
			mac.Initialize(m_macKeys[i]);
			mac.BlockUpdate(m_macData[i], 0, m_macData[i].size());
			std::vector<byte> macV(mac.MacSize());
			mac.DoFinal(macV, 0);

			if (Expected[i] != macV)
				throw std::string("Keccak HMAC: Expected hash is not equal!");
		}

		// test truncated keys
		CEX::Mac::HMAC mac2(Digest);
		mac2.Initialize(m_truncKey);
		mac2.BlockUpdate(m_truncData, 0, m_truncData.size());
		mac2.DoFinal(macV2, 0);

		for (unsigned int i = 0; i != TruncExpected.size(); i++)
		{
			if (macV2[i] != TruncExpected[i])
				throw std::string("Keccak HMAC: Expected hash is not equal!");
		}
	}

	void KeccakTest::Initialize()
	{
		const char* messagesEnc[3] =
		{
			(""),
			("54686520717569636b2062726f776e20666f78206a756d7073206f76657220746865206c617a7920646f67"),
			("54686520717569636b2062726f776e20666f78206a756d7073206f76657220746865206c617a7920646f672e")
		};
		HexConverter::Decode(messagesEnc, 3, m_messages);

		const char* expected224Enc[6] =
		{
			("f71837502ba8e10837bdd8d365adb85591895602fc552b48b7390abd"),
			("310aee6b30c47350576ac2873fa89fd190cdc488442f3ef654cf23fe"),
			("c59d4eaeac728671c635ff645014e2afa935bebffdb5fbd207ffdeab"),
			("f621e11c142fbf35fa8c22841c3a812ba1e0151be4f38d80b9f1ff53"),
			("68b5fc8c87193155bba68a2485377e809ee4f81a85ef023b9e64add0"),
			("c42e4aee858e1a8ad2976896b9d23dd187f64436ee15969afdbc68c5")
		};
		HexConverter::Decode(expected224Enc, 6, m_expected224);

		const char* expected256Enc[6] =
		{
			("c5d2460186f7233c927e7db2dcc703c0e500b653ca82273b7bfad8045d85a470"),
			("4d741b6f1eb29cb2a9b9911c82f56fa8d73b04959d3d9d222895df6c0b28aa15"),
			("578951e24efd62a3d63a86f7cd19aaa53c898fe287d2552133220370240b572d"),
			("0047a916daa1f92130d870b542e22d3108444f5a7e4429f05762fb647e6ed9ed"),
			("db368762253ede6d4f1db87e0b799b96e554eae005747a2ea687456ca8bcbd03"),
			("5f313c39963dcf792b5470d4ade9f3a356a3e4021748690a958372e2b06f82a4")
		};
		HexConverter::Decode(expected256Enc, 6, m_expected256);

		const char* expected288Enc[6] =
		{
			("6753e3380c09e385d0339eb6b050a68f66cfd60a73476e6fd6adeb72f5edd7c6f04a5d01"),
			("0bbe6afae0d7e89054085c1cc47b1689772c89a41796891e197d1ca1b76f288154933ded"),
			("82558a209b960ddeb531e6dcb281885b2400ca160472462486e79f071e88a3330a8a303d"),
			("94049e1ad7ef5d5b0df2b880489e7ab09ec937c3bfc1b04470e503e1ac7b1133c18f86da"),
			("a9cb5a75b5b81b7528301e72553ed6770214fa963956e790528afe420de33c074e6f4220"),
			("eadaf5ba2ad6a2f6f338fce0e1efdad2a61bb38f6be6068b01093977acf99e97a5d5827c")
		};
		HexConverter::Decode(expected288Enc, 6, m_expected288);

		const char* expected384Enc[6] =
		{
			("2c23146a63a29acf99e73b88f8c24eaa7dc60aa771780ccc006afbfa8fe2479b2dd2b21362337441ac12b515911957ff"),
			("283990fa9d5fb731d786c5bbee94ea4db4910f18c62c03d173fc0a5e494422e8a0b3da7574dae7fa0baf005e504063b3"),
			("9ad8e17325408eddb6edee6147f13856ad819bb7532668b605a24a2d958f88bd5c169e56dc4b2f89ffd325f6006d820b"),
			("c704cfe7a1a53208ca9526cd24251e0acdc252ecd978eee05acd16425cfb404ea81f5a9e2e5e97784d63ee6a0618a398"),
			("d4fe8586fd8f858dd2e4dee0bafc19b4c12b4e2a856054abc4b14927354931675cdcaf942267f204ea706c19f7beefc4"),
			("9b7168b4494a80a86408e6b9dc4e5a1837c85dd8ff452ed410f2832959c08c8c0d040a892eb9a755776372d4a8732315")
		};
		HexConverter::Decode(expected384Enc, 6, m_expected384);

		const char* expected512Enc[6] =
		{
			("0eab42de4c3ceb9235fc91acffe746b29c29a8c366b7c60e4e67c466f36a4304c00fa9caf9d87976ba469bcbe06713b435f091ef2769fb160cdab33d3670680e"),
			("d135bb84d0439dbac432247ee573a23ea7d3c9deb2a968eb31d47c4fb45f1ef4422d6c531b5b9bd6f449ebcc449ea94d0a8f05f62130fda612da53c79659f609"),
			("ab7192d2b11f51c7dd744e7b3441febf397ca07bf812cceae122ca4ded6387889064f8db9230f173f6d1ab6e24b6e50f065b039f799f5592360a6558eb52d760"),
			("34341ead153aa1d1fdcf6cf624c2b4f6894b6fd16dc38bd4ec971ac0385ad54fafcb2e0ed86a1e509456f4246fdcb02c3172824cd649d9ad54c51f7fb49ea67c"),
			("dc44d4f4d36b07ab5fc04016cbe53548e5a7778671c58a43cb379fd00c06719b8073141fc22191ffc3db5f8b8983ae8341fa37f18c1c969664393aa5ceade64e"),
			("3e122edaf37398231cfaca4c7c216c9d66d5b899ec1d7ac617c40c7261906a45fc01617a021e5da3bd8d4182695b5cb785a28237cbb167590e34718e56d8aab8")
		};
		HexConverter::Decode(expected512Enc, 6, m_expected512);

		const char* macKeysEnc[7] =
		{
			("0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b"),
			("4a656665"),
			("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"),
			("0102030405060708090a0b0c0d0e0f10111213141516171819"),
			("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" \
				"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"),
			("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" \
				"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"),
			("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" \
				"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa")
		};
		HexConverter::Decode(macKeysEnc, 7, m_macKeys);

		const char* macDataEnc[7] =
		{
			("4869205468657265"),
			("7768617420646f2079612077616e7420666f72206e6f7468696e673f"),
			("dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd"),
			("cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd"),
			("54657374205573696e67204c6172676572205468616e20426c6f636b2d53697a65204b6579202d2048617368204b6579204669727374"),
			("5468697320697320612074657374207573696e672061206c6172676572207468616e20626c6f636b2d73697a65206b657920616e642061206c61726765722074" \
				"68616e20626c6f636b2d73697a6520646174612e20546865206b6579206e6565647320746f20626520686173686564206265666f7265206265696e6720757365" \
				"642062792074686520484d414320616c676f726974686d2e"),
			("5468697320697320612074657374207573696e672061206c6172676572207468616e20626c6f636b2d73697a65206b657920616e642061206c61726765722074" \
				"68616e20626c6f636b2d73697a6520646174612e20546865206b6579206e6565647320746f20626520686173686564206265666f7265206265696e6720757365\n" \
				"642062792074686520484d414320616c676f726974686d2e")
		};
		HexConverter::Decode(macDataEnc, 7, m_macData);

		const char* mac224Enc[7] =
		{
			("b73d595a2ba9af815e9f2b4e53e78581ebd34a80b3bbaac4e702c4cc"),
			("e824fec96c074f22f99235bb942da1982664ab692ca8501053cbd414"),
			("770df38c99d6e2bacd68056dcfe07d4c89ae20b2686a6185e1faa449"),
			("305a8f2dfb94bad28861a03cbc4d590febe775c58cb4961c28428a0b"),
			("e7a52dfa45f95a217c100066b239aa8ad519be9b35d667268b1b57ff"),
			("ba13009405a929f398b348885caa5419191bb948ada32194afc84104"),
			("92649468be236c3c72c189909c063b13f994be05749dc91310db639e")
		};
		HexConverter::Decode(mac224Enc, 7, m_mac224);

		const char* mac256Enc[7] =
		{
			("9663d10c73ee294054dc9faf95647cb99731d12210ff7075fb3d3395abfb9821"),
			("aa9aed448c7abc8b5e326ffa6a01cdedf7b4b831881468c044ba8dd4566369a1"),
			("95f43e50f8df80a21977d51a8db3ba572dcd71db24687e6f86f47c1139b26260"),
			("6331ba9b4af5804a68725b3663eb74814494b63c6093e35fb320a85d507936fd"),
			("b4d0cdee7ec2ba81a88b86918958312300a15622377929a054a9ce3ae1fac2b6"),
			("1fdc8cb4e27d07c10d897dec39c217792a6e64fa9c63a77ce42ad106ef284e02"),
			("fdaa10a0299aecff9bb411cf2d7748a4022e4a26be3fb5b11b33d8c2b7ef5484")
		};
		HexConverter::Decode(mac256Enc, 7, m_mac256);

		const char* mac384Enc[7] =
		{
			("892dfdf5d51e4679bf320cd16d4c9dc6f749744608e003add7fba894acff87361efa4e5799be06b6461f43b60ae97048"),
			("5af5c9a77a23a6a93d80649e562ab77f4f3552e3c5caffd93bdf8b3cfc6920e3023fc26775d9df1f3c94613146ad2c9d"),
			("4243c29f2201992ff96441e3b91ff81d8c601d706fbc83252684a4bc51101ca9b2c06ddd03677303c502ac5331752a3c"),
			("b730724d3d4090cda1be799f63acbbe389fef7792fc18676fa5453aab398664650ed029c3498bbe8056f06c658e1e693"),
			("d62482ef601d7847439b55236e9679388ffcd53c62cd126f39be6ea63de762e26cd5974cb9a8de401b786b5555040f6f"),
			("4860ea191ac34994cf88957afe5a836ef36e4cc1a66d75bf77defb7576122d75f60660e4cf731c6effac06402787e2b9"),
			("fe9357e3cfa538eb0373a2ce8f1e26ad6590afdaf266f1300522e8896d27e73f654d0631c8fa598d4bb82af6b744f4f5")
		};
		HexConverter::Decode(mac384Enc, 7, m_mac384);

		const char* mac512Enc[7] =
		{
			("8852c63be8cfc21541a4ee5e5a9a852fc2f7a9adec2ff3a13718ab4ed81aaea0b87b7eb397323548e261a64e7fc75198f6663a11b22cd957f7c8ec858a1c7755"),
			("c2962e5bbe1238007852f79d814dbbecd4682e6f097d37a363587c03bfa2eb0859d8d9c701e04cececfd3dd7bfd438f20b8b648e01bf8c11d26824b96cebbdcb"),
			("eb0ed9580e0ec11fc66cbb646b1be904eaff6da4556d9334f65ee4b2c85739157bae9027c51505e49d1bb81cfa55e6822db55262d5a252c088a29a5e95b84a66"),
			("b46193bb59f4f696bf702597616da91e2a4558a593f4b015e69141ba81e1e50ea580834c2b87f87baa25a3a03bfc9bb389847f2dc820beae69d30c4bb75369cb"),
			("d05888a6ebf8460423ea7bc85ea4ffda847b32df32291d2ce115fd187707325c7ce4f71880d91008084ce24a38795d20e6a28328a0f0712dc38253370da3ebb5"),
			("2c6b9748d35c4c8db0b4407dd2ed2381f133bdbd1dfaa69e30051eb6badfcca64299b88ae05fdbd3dd3dd7fe627e42e39e48b0fe8c7f1e85f2dbd52c2d753572"),
			("6adc502f14e27812402fc81a807b28bf8a53c87bea7a1df6256bf66f5de1a4cb741407ad15ab8abc136846057f881969fbb159c321c904bfb557b77afb7778c8")
		};
		HexConverter::Decode(mac512Enc, 7, m_mac512);

		HexConverter::Decode("0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c", m_truncKey);
		HexConverter::Decode("546573742057697468205472756e636174696f6e", m_truncData);
		HexConverter::Decode("f52bbcfd654264e7133085c5e69b72c3", m_trunc224);
		HexConverter::Decode("745e7e687f8335280d54202ef13cecc6", m_trunc256);
		HexConverter::Decode("fa9aea2bc1e181e47cbb8c3df243814d", m_trunc384);
		HexConverter::Decode("04c929fead434bba190dacfa554ce3f5", m_trunc512);
		HexConverter::Decode("61626364656667686263646566676869636465666768696a6465666768696a6b65666768696a6b6c666768696a6b6c6d6768696a6b6c6d6e68696a6b6c6d6e6f", m_xtremeData);
	}

	void KeccakTest::OnProgress(char* Data)
	{
		m_progressEvent(Data);
	}
}