#ifndef CEX_NTRUPRIVATEKEY_H
#define CEX_NTRUPRIVATEKEY_H

#include "CexDomain.h"
#include "IAsymmetricKey.h"
#include "NTRUParams.h"

NAMESPACE_ASYMMETRICKEY

using Enumeration::NTRUParams;

/// <summary>
/// A ModuleLWE Private Key container
/// </summary>
class NTRUPrivateKey final : public IAsymmetricKey
{
private:

	bool m_isDestroyed;
	std::vector<byte> m_rCoeffs;
	NTRUParams m_mlweParameters;

public:

	//~~~Constructor~~~//

	/// <summary>
	/// Copy constructor: copy is restricted, this function has been deleted
	/// </summary>
	NTRUPrivateKey(const NTRUPrivateKey&) = delete;

	/// <summary>
	/// Copy operator: copy is restricted, this function has been deleted
	/// </summary>
	NTRUPrivateKey& operator=(const NTRUPrivateKey&) = delete;

	/// <summary>
	/// Default constructor: default is restricted, this function has been deleted
	/// </summary>
	NTRUPrivateKey() = delete;

	/// <summary>
	/// Initialize this class with parameters
	/// </summary>
	/// 
	/// <param name="Parameters">The cipher parameter enumeration name</param>
	/// <param name="R">The private key polynomial</param>
	NTRUPrivateKey(NTRUParams Parameters, std::vector<byte> &R);

	/// <summary>
	/// Initialize this class with a serialized private key
	/// </summary>
	/// 
	/// <param name="KeyStream">The serialized private key</param>
	explicit NTRUPrivateKey(const std::vector<byte> &KeyStream);

	/// <summary>
	/// Destructor: finalize this class
	/// </summary>
	~NTRUPrivateKey() override;

	//~~~Accessors~~~//

	/// <summary>
	/// Read Only: The private keys cipher type name
	/// </summary>
	const AsymmetricEngines CipherType() override;

	/// <summary>
	/// Read Only: The keys type-name
	/// </summary>
	const AsymmetricKeyTypes KeyType() override;

	/// <summary>
	/// Read Only: The cipher parameters enumeration name
	/// </summary>
	const NTRUParams Parameters();

	/// <summary>
	/// Read Only: the private key polynomial R
	/// </summary>
	const std::vector<byte> &R();

	//~~~Public Functions~~~//

	/// <summary>
	/// Release all resources associated with the object; optional, called by the finalizer
	/// </summary>
	void Destroy() override;

	/// <summary>
	/// Serialize a private key to a byte array
	/// </summary>
	std::vector<byte> ToBytes() override;
};

NAMESPACE_ASYMMETRICKEYEND
#endif
