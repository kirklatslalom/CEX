#include "PrngFromName.h"
#include "BlockCiphers.h"
#include "BCR.h"
#include "DCR.h"
#include "HCR.h"

NAMESPACE_HELPER

IPrng* PrngFromName::GetInstance(Prngs PrngType, Providers ProviderType, Digests DigestType)
{
	if (PrngType == Prngs::None)
	{
		CryptoException("PrngFromName:GetPrng", "Prng type can not be none!");
	}
	if (ProviderType == Providers::None)
	{
		CryptoException("PrngFromName:GetPrng", "Prng type can not be none!");
	}
	if (PrngType != Prngs::BCR && DigestType == Digests::None)
	{
		CryptoException("PrngFromName:GetPrng", "Digest type can not be none when using Digest or HMAC based rng!");
	}

	IPrng* rngPtr;

	try
	{
		switch (PrngType)
		{
			case Prngs::BCR:
			{
				rngPtr = new Prng::BCR(Enumeration::BlockCiphers::AHX, ProviderType);
				break;
			}
			case Prngs::DCR:
			{
				rngPtr = new Prng::DCR(DigestType, ProviderType);
				break;
			}
			case Prngs::HCR:
			{
				rngPtr = new Prng::HCR(DigestType, ProviderType);
				break;
			}
			default:
			{
				throw CryptoException("PrngFromName:GetPrng", "The specified prng type is unrecognized!");
			}
		}
	}
	catch (const std::exception &ex)
	{
		throw CryptoException("PrngFromName:GetInstance", "The prng is unavailable!", std::string(ex.what()));
	}

	return rngPtr;
}

NAMESPACE_HELPEREND