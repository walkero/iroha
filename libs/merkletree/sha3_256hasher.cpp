#include "cryptography/ed25519_sha3_impl/internal/sha3_hash.hpp"
#include "merkletree/serial_hasher.h"

namespace iroha {

  class OneHasher : public merkletree::SerialHasher {
   private:
    std::string hash_;

   public:
    virtual void Reset() { hash_ = ""; };

    virtual void Update(const std::string &data) {
      hash_ = sha3_256(hash_ + data).to_string();
    }

    virtual std::string Final() {
      auto res = hash_;
      Reset();
      return res;
    }

    virtual size_t DigestSize() const { return hash256_t::size(); }

    virtual std::unique_ptr<SerialHasher> Create() const {
      return std::move(std::unique_ptr<SerialHasher>{new OneHasher()});
    }
  };
}
