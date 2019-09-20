# Learnoran
CPU Parallel Homomorphic Machine Learning Framework for C++

## What is Learnoran?

Learnoran is a Machine Learning platform capable of working with homomorphically encrypted data. The framework currently enables the user to perform training, and prediction on the linear regression model (which will be expanded to polynomial regressors soon). The ultimate aim is to have neural networks to operate on ciphertexts as well. Learnoran currently implements fully connected neural networks with sigmoidal transfer functions; however homomorphic capabilities haven't been implemented for ANNs yet.
<br/>
<br/>
Learnoran uses Microsoft SEAL v3 under the hood.

## Usage

Examples are provided below.

### Homomorphic linear regression training & predicting
```cpp
#include "encryption_manager.hpp"
#include "decryption_manager.hpp"
#include "linear_model.hpp"
#include "dataframe.hpp"

using namespace Learnoran;

EncryptedNumber encrypted_linear_regression(const Dataframe<EncryptedNumber> & df, 
  const unordered_map<string, EncryptedNumber> test_features, 
  shared_ptr<EncryptionManager> enc_manager) {
	LinearModel regressor(enc_manager);

	regressor.fit(df, 3, 0.00001);

	EncryptedNumber prediction = regressor.predict(test_features);

	return prediction;
}

int main() {
  // ...
  // read a dataset as std::pair<std::vector<std::vector<double>>, std::vector<double>>
  // (training data, training labels), and construct a Learnoran::Dataframe object,
  // Initialize EncryptionManager and DecryptionManager objects
  
  Dataframe<EncryptedNumber> * encrypted_dataframe = new Dataframe<EncryptedNumber>(encryption_manager->encrypt_dataframe(df));
  
  EncryptedNumber linear_reg_enc_pred = encrypted_linear_regressor_test(*enc_df, encrypted_features, enc_manager);
  cout << "encrypted linear regressor prediction result: " << dec_manager.decrypt(linear_reg_enc_pred) << endl;

  return 0;
}
```
