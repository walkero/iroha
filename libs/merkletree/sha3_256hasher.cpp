class OneHasher : public SerialHasher {
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

    hash256_t generate_merkle_root(
        const std::vector<iroha::model::Transaction> &transactions) {
      auto logger = logger::log("Simulator");
      std::unique_ptr<SerialHasher> hasher{new OneHasher()};
      MerkleTree tree{std::move(hasher)};
      for (int i = 1; i < transactions.size(); i++) {
        tree.AddLeaf(std::to_string(i));
      }
      auto res = tree.CurrentRoot();
      // while (res.size() < hash256_t::size()) {
      //   res += "1";
      // }
      return hash256_t::from_string(res);
    }
