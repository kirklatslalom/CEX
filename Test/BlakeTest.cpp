#include "BlakeTest.h"
#include "Blake256.h"
#include "Blake512.h"

namespace Test
{
	std::string BlakeTest::Run()
	{
		try
		{
			Initialize();

			CEX::Digest::Blake256* dgt256 = new CEX::Digest::Blake256();
			CompareVector(dgt256, m_message[0], m_expected[0]);
			CompareVector(dgt256, m_message[1], m_expected[1]);
			CompareVector(dgt256, m_message[2], m_expected[2]);
			CompareVector(dgt256, m_message[3], m_expected[3]);
			CompareVector(dgt256, m_message[4], m_expected[4]);
			CompareVector(dgt256, m_message[5], m_expected[5]);
			CompareVector(dgt256, m_message[6], m_expected[6]);
			CompareVector(dgt256, m_message[7], m_expected[7]);
			CompareVector(dgt256, m_message[8], m_expected[8]);
			CompareVector(dgt256, m_message[9], m_expected[9]);
			delete dgt256;
			OnProgress("Passed Blake 256 vector tests..");

			CEX::Digest::Blake512* dgt512 = new CEX::Digest::Blake512();
			CompareVector(dgt512, m_message[10], m_expected[10]);
			CompareVector(dgt512, m_message[11], m_expected[11]);
			CompareVector(dgt512, m_message[12], m_expected[12]);
			CompareVector(dgt512, m_message[13], m_expected[13]);
			CompareVector(dgt512, m_message[14], m_expected[14]);
			CompareVector(dgt512, m_message[15], m_expected[15]);
			CompareVector(dgt512, m_message[16], m_expected[16]);
			CompareVector(dgt512, m_message[17], m_expected[17]);
			CompareVector(dgt512, m_message[18], m_expected[18]);
			CompareVector(dgt512, m_message[19], m_expected[19]);
			delete dgt512;
			OnProgress("Passed Blake 512 vector tests..");

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

	void BlakeTest::CompareVector(CEX::Digest::IDigest *Digest, std::vector<byte> Input, std::vector<byte> Expected)
	{
		std::vector<byte> hash(Digest->DigestSize(), 0);

		Digest->Reset();

		if (Input.size() != 0)
			Digest->BlockUpdate(Input, 0, Input.size());

		Digest->DoFinal(hash, 0);

		if (Expected != hash)
			throw std::string("Blake: Expected hash is not equal!");

		Digest->ComputeHash(Input, hash);

		if (Expected != hash)
			throw std::string("Blake: Expected hash is not equal!");
	}

	void BlakeTest::Initialize()
	{
		const char* expectedEncoded[20] =
		{
			("000E0F08E2EE912478B77004EC62845B5E01418F03837B76CBDC8B1FB0480322"),//256
			("98D8F1C37D86DC0FC7260C235074C28F404087D1A59EAC521B9BE4F06FEB133C"),
			("29B7381807B1E95958ADACA55CB080938E7715C0DE4D7B7BB75402C67B976CAE"),
			("616483B40DFF92345808ABB12BA3F9ADDAE204952A8C0CE5902AA039E2436FD9"),
			("1685B8F40F0A7C0A23DF809A55E5DC59E46F9BA0C5E0B3A33B7832F62535B4B2"),
			("BB7F033549A3BDCFA3F06C0B0320D2B49EA63EC1F912836339D707430974B178"),
			("F7596D7A26126B17EC4CFCD381F32DD24244C8A176D04125806B31605F0FF2CD"),
			("73CD549C862952BF48CA5AA516F74E9739F5E4420D13AD22AC3717709AF6C5F8"),
			("37EF34B4836ECF0188E06798BA388DF12B213E13325A4E0E3076DB02263AD11D"),
			("348B7A102B867C2978905BD68535F9672E527932AC45EC2A045AC417020F2C14"),
			("36D38FB9DDA1EF575E316D5319D79345457602F581448E09372715AA096B22897B0B70FA3C9F835EE4F3D31B9DE308E95A0477158D3254EEA9EE6372534D682A"),//512
			("01E3AEA9D5851A374AFB51FB50BDC6648BC470510292F98BF69B44E30FA4929710AAE5D4200C016800CFBEA54590FA714563C5FB8ACD9C5E612C758DA4873804"),
			("64D8ACAFA4AF54B1F4F3F3B3D0D8C5940490BD860E5661BC97EE250AD322342CEB7A1FBFAC8FE8A9BB12D2A24047B948C73D02B3E35A22C570E4C91AC767D205"),
			("7BCA565068AFFD089194F3F0F6770A07A8FF9B8D4022547CA5DCCAAA7505C8F1C9153C282744BE3FE082C62BBD88A396E4F3C9C046BBBDE2BA71DE30DF6448FA"),
			("460788157FD278E1EC96BEE87D57D8C1400C1B1ABD40B3F03074225A13D35526A5CC28E682521349FDCB161CCA7D67917DD44C0B148B1B23AE7093549A04919A"),
			("DC4D1BC4DABCBCF680F28FF7DC12459F23CFFB13652B2334CF51594E5818864F575BAF4D899522ABE6E3B7EBB8315A29E85EE33BEE088D410CCB300147B91527"),
			("A8407FFD7A3A5F73D20FFA718A388B96A7F51426E6805FCDF0C5CCA59C86FB06159073A17F06F358F15EF9330EA5035F4A4F8826DC2040336926F007D8A115F2"),
			("1E22CAF5A0021782DAF71DA59BE0E6CDB133F27D66AC4799E95FBA3324473353E940006B1EEAA376281A1FCFFA86EB49E0DBE66DED5BB88EA8D5F6AD447F1C0B"),
			("45C48A199A992BD22291A2ADF54FF7A5379348AF571FE0E4AE51DBD57D1A7B73A13419AC55737013649F485551EB39A0E51BA8E216AC6E75ADA136B38F019BDE"),
			("C546CC110B9175896713BEC3F7D158A4ED26081BCAF9CA0DC91EA91CAA9381EA84E9A4758BA15CC3D44F1693FF77C68173122B9293F81133D480C75BA77842B0"),
		};
		HexConverter::Decode(expectedEncoded, 20, m_expected);

		const char* messageEncoded[20] =
		{
			("40"),//256
			("3197"),
			("4D22DF"),
			("6CCD13D7"),
			("D8D9C639AE"),
			("FBC4AB9E4B2F"),
			("D19BEB74934639"),
			("48DE5F4845276C8F"),
			("2E86993AA9577E0176DEE507BF649F9358EEBE67CEAFA2D4C10ACE8D4BE895AD"),
			("EBECF3C5A7476DBB35D348DBEC5A0EB7B3A5499BE07B8FCD489A7B56E07F0A91D196"),
			("40"),//512
			("3197"),
			("4D22DF"),
			("6CCD13D7"),
			("D8D9C639AE"),
			("FBC4AB9E4B2F"),
			("D19BEB74934639"),
			("48DE5F4845276C8F"),
			("F1F45C8C41D6B06FC5DA2E6242A6F966EAB82447BC38D38FC50D1B61D29017AD0DB36E2C39C5FB2985AA495F265D01CED6EE77581A72C528FE78288940ADF8"),
			("E52397A1D04ED8EC1CFB4D8F7A8566AA28DDF8B6F959D460D47A3A5A06B655BACE1998E4250666361F2D449264AE41AABA9341C50A00F3DE354C1468E408CAA4"),
		};
		HexConverter::Decode(messageEncoded, 20, m_message);
	}

	void BlakeTest::OnProgress(char* Data)
	{
		m_progressEvent(Data);
	}
}