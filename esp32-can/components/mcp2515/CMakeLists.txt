idf_component_register(SRCS "mcp2515.cpp" "spi.cpp"
                    INCLUDE_DIRS "include"
                    REQUIRES driver freertos
                    )
