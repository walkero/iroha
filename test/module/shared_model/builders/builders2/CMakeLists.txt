addtest(transaction_builder_test
        transaction_builder_test.cpp
)

target_link_libraries(transaction_builder_test
        shared_model_proto_builders
        )
