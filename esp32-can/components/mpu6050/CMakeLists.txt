idf_component_register(SRCS "i2c.cpp" "mpu6050.cpp"
                    INCLUDE_DIRS "include"
                    REQUIRES driver freertos
                    )
