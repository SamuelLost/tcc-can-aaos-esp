clang-format-12 -i -style=Google -sort-includes=true components/mpu6050/include/*.h components/mpu6050/*.cpp main/*.cpp components/gprs/include/*.h components/gprs/*.cpp

#find . -name "build" -prune -o \( -name "*.cpp" -or -name "*.h" \) -type f -print0 | xargs -0 clang-format-12 -i -style=Google -sort-includes=true