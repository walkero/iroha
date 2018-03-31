addtest(blob_test blob_test.cpp)
target_link_libraries(blob_test
        shared_model_cryptography_model
        )

if(NOT SHARED_MODEL_DISABLE_COMPATIBILITY)
    SET(LINK_OLD_MODEL model)
endif()

addtest(crypto_usage_test crypto_usage_test.cpp)
target_link_libraries(crypto_usage_test
        shared_model_cryptography
        schema
        iroha_amount
        ${LINK_OLD_MODEL}
        )
