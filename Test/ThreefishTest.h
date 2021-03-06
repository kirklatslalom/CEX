#ifndef CEXTEST_THREEFISHTEST_H
#define CEXTEST_THREEFISHTEST_H

#include "ITest.h"
#include "../CEX/IStreamCipher.h"

namespace Test
{
	using Cipher::Symmetric::Stream::IStreamCipher;

	/// <summary>
	/// The Threefish implementation KAT, stress, and exception handling tests
	/// </summary>
	/// 
	/// <remarks>
	/// <description>References:</description>
	/// <list type="number">
	/// <item><description>The Skein Hash Function Family <a href="https://www.schneier.com/academic/paperfiles/skein1.3.pdf">Skein V1.1</a>.</description></item>
	/// <item><description>NIST Round 3 <a href="https://www.schneier.com/academic/paperfiles/skein-1.3-modifications.pdf">Tweak Description</a>.</description></item>
	/// <item><description>Skein <a href="https://www.schneier.com/academic/paperfiles/skein-proofs.pdf">Provable Security</a> Support for the Skein Hash Family.</description></item>
	/// <item><description>NIST <a href="http://nvlpubs.nist.gov/nistpubs/ir/2012/NIST.IR.7896.pdf">SHA3 Third-Round Report</a> of the SHA-3 Cryptographic Hash Algorithm Competition>.</description></item>
	/// </list>
	/// </remarks>
	class ThreefishTest final : public ITest
	{
	private:

		static const std::string DESCRIPTION;
		static const std::string FAILURE;
		static const std::string SUCCESS;
		static const size_t MAXM_ALLOC = 262140;
		static const size_t TEST_CYCLES = 100;

		std::vector<std::vector<byte>> m_expected;
		std::vector<std::vector<byte>> m_message;
		TestEventHandler m_progressEvent;

	public:

		//~~~Constructor~~~//

		/// <summary>
		/// Original known answer tests for the 256, 512, and 1024 bit versions of Threefish
		/// </summary>
		ThreefishTest();

		/// <summary>
		/// Destructor
		/// </summary>
		~ThreefishTest();

		//~~~Accessors~~~//

		/// <summary>
		/// Get: The test description
		/// </summary>
		const std::string Description() override;

		/// <summary>
		/// Progress return event callback
		/// </summary>
		TestEventHandler &Progress() override;

		/// <summary>
		/// Start the tests
		/// </summary>
		std::string Run() override;

		//~~~Public Functions~~~//

		/// <summary>
		/// Tests the the cipher transformation using each supported authentication mode
		/// </summary>
		/// 
		/// <param name="Cipher1">The authenticated cipher instance pointer</param>
		/// <param name="Cipher2">The unauthenticated cipher instance used for transformation comparison</param>
		void Authentication(IStreamCipher* Cipher);

		/// <summary>
		/// Test exception handlers for correct execution
		/// </summary>
		/// 
		/// <param name="Cipher">The cipher instance pointer</param>
		void Exception(IStreamCipher* Cipher);

		/// <summary>
		/// Compare known answer test vectors to authenticated and standard cipher output
		/// </summary>
		/// 
		/// <param name="Cipher">The cipher instance pointer</param>
		/// <param name="Input">The input test message</param>
		/// <param name="Expected">The expected output vector</param>
		void Kat(IStreamCipher* Cipher, std::vector<byte> &Input, std::vector<byte> &Expected);

		/// <summary>
		/// Compares synchronous to parallel processed random-sized, pseudo-random array transformations and their inverse in a looping [TEST_CYCLES] stress-test
		/// </summary>
		/// 
		/// <param name="Cipher">The cipher instance pointer</param>
		void Parallel(IStreamCipher* Cipher);

		/// <summary>
		/// Compare Threefish-256 compact and unrolled permutation functions for equivalence
		/// </summary>
		void Permutation256();

		/// <summary>
		/// Compare Threefish-512 compact and unrolled permutation functions for equivalence
		/// </summary>
		void Permutation512();

		/// <summary>
		/// Compare Threefish-1024 compact and unrolled permutation functions for equivalence
		/// </summary>
		void Permutation1024();

		/// <summary>
		/// Test transformation and inverse with random in a looping [TEST_CYCLES] stress-test
		/// </summary>
		/// 
		/// <param name="Cipher">The cipher instance pointer</param>
		void Stress(IStreamCipher* Cipher);

	private:

		void Initialize();
		void OnProgress(std::string Data);
	};
}

#endif
