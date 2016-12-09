/*
 *  Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License
 */

#ifndef __RUNTIME_PREPROCESSOR_H__
#define __RUNTIME_PREPROCESSOR_H__

#define STRINGIFY_(x) #x
#define STRINGIFY(x)  STRINGIFY_(x)

#define CONCATENATE(arg1, arg2)   CONCATENATE1(arg1, arg2)
#define CONCATENATE1(arg1, arg2)  CONCATENATE2(arg1, arg2)
#define CONCATENATE2(arg1, arg2)  arg1##arg2

#define VAR_ARGS_SIZE(...)  VAR_ARGS_SIZE_((VAR_ARGS_EMPTY ## __VA_ARGS__ , FOR_EACH_VAR_ARGS_RSEQ_N()))
#define VAR_ARGS_SIZE_(ARGS) FOR_EACH_VAR_ARGS_ARG_N ARGS
#define VAR_ARGS_EMPTY FOR_EACH_VAR_ARGS_RSEQ_N()

#define FOR_EACH_VAR_ARGS_ARG_N(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, N, ...) N
#define FOR_EACH_VAR_ARGS_RSEQ_N() 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define FOR_EACH_VAR_ARGS_1(what, x, ...)  what(x)
#define FOR_EACH_VAR_ARGS_2(what, x, ...)  what(x); FOR_EACH_VAR_ARGS_1(what, __VA_ARGS__)
#define FOR_EACH_VAR_ARGS_3(what, x, ...)  what(x); FOR_EACH_VAR_ARGS_2(what, __VA_ARGS__)
#define FOR_EACH_VAR_ARGS_4(what, x, ...)  what(x); FOR_EACH_VAR_ARGS_3(what, __VA_ARGS__)
#define FOR_EACH_VAR_ARGS_5(what, x, ...)  what(x); FOR_EACH_VAR_ARGS_4(what, __VA_ARGS__)
#define FOR_EACH_VAR_ARGS_6(what, x, ...)  what(x); FOR_EACH_VAR_ARGS_5(what, __VA_ARGS__)
#define FOR_EACH_VAR_ARGS_7(what, x, ...)  what(x); FOR_EACH_VAR_ARGS_6(what, __VA_ARGS__)
#define FOR_EACH_VAR_ARGS_8(what, x, ...)  what(x); FOR_EACH_VAR_ARGS_7(what, __VA_ARGS__)
#define FOR_EACH_VAR_ARGS_9(what, x, ...)  what(x); FOR_EACH_VAR_ARGS_8(what, __VA_ARGS__)
#define FOR_EACH_VAR_ARGS_10(what, x, ...) what(x); FOR_EACH_VAR_ARGS_9(what, __VA_ARGS__)
#define FOR_EACH_VAR_ARGS_11(what, x, ...) what(x); FOR_EACH_VAR_ARGS_10(what, __VA_ARGS__)
#define FOR_EACH_VAR_ARGS_12(what, x, ...) what(x); FOR_EACH_VAR_ARGS_11(what, __VA_ARGS__)
#define FOR_EACH_VAR_ARGS_13(what, x, ...) what(x); FOR_EACH_VAR_ARGS_12(what, __VA_ARGS__)
#define FOR_EACH_VAR_ARGS_14(what, x, ...) what(x); FOR_EACH_VAR_ARGS_13(what, __VA_ARGS__)
#define FOR_EACH_VAR_ARGS_15(what, x, ...) what(x); FOR_EACH_VAR_ARGS_14(what, __VA_ARGS__)
#define FOR_EACH_VAR_ARGS_16(what, x, ...) what(x); FOR_EACH_VAR_ARGS_15(what, __VA_ARGS__)
#define FOR_EACH_VAR_ARGS_17(what, x, ...) what(x); FOR_EACH_VAR_ARGS_16(what, __VA_ARGS__)
#define FOR_EACH_VAR_ARGS_18(what, x, ...) what(x); FOR_EACH_VAR_ARGS_17(what, __VA_ARGS__)
#define FOR_EACH_VAR_ARGS_19(what, x, ...) what(x); FOR_EACH_VAR_ARGS_18(what, __VA_ARGS__)
#define FOR_EACH_VAR_ARGS_20(what, x, ...) what(x); FOR_EACH_VAR_ARGS_19(what, __VA_ARGS__)
#define FOR_EACH_VAR_ARGS_21(what, x, ...) what(x); FOR_EACH_VAR_ARGS_20(what, __VA_ARGS__)
#define FOR_EACH_VAR_ARGS_22(what, x, ...) what(x); FOR_EACH_VAR_ARGS_21(what, __VA_ARGS__)
#define FOR_EACH_VAR_ARGS_23(what, x, ...) what(x); FOR_EACH_VAR_ARGS_22(what, __VA_ARGS__)
#define FOR_EACH_VAR_ARGS_24(what, x, ...) what(x); FOR_EACH_VAR_ARGS_23(what, __VA_ARGS__)
#define FOR_EACH_VAR_ARGS_25(what, x, ...) what(x); FOR_EACH_VAR_ARGS_24(what, __VA_ARGS__)
#define FOR_EACH_VAR_ARGS_26(what, x, ...) what(x); FOR_EACH_VAR_ARGS_25(what, __VA_ARGS__)
#define FOR_EACH_VAR_ARGS_27(what, x, ...) what(x); FOR_EACH_VAR_ARGS_26(what, __VA_ARGS__)
#define FOR_EACH_VAR_ARGS_28(what, x, ...) what(x); FOR_EACH_VAR_ARGS_27(what, __VA_ARGS__)
#define FOR_EACH_VAR_ARGS_29(what, x, ...) what(x); FOR_EACH_VAR_ARGS_28(what, __VA_ARGS__)
#define FOR_EACH_VAR_ARGS_30(what, x, ...) what(x); FOR_EACH_VAR_ARGS_29(what, __VA_ARGS__)
#define FOR_EACH_VAR_ARGS_31(what, x, ...) what(x); FOR_EACH_VAR_ARGS_30(what, __VA_ARGS__)
#define FOR_EACH_VAR_ARGS_32(what, x, ...) what(x); FOR_EACH_VAR_ARGS_31(what, __VA_ARGS__)

#define FOR_EACH_VAR_ARGS_(N, what, ...)   CONCATENATE(FOR_EACH_VAR_ARGS_, N)(what, __VA_ARGS__)
#define FOR_EACH_VAR_ARGS(what, ...)       FOR_EACH_VAR_ARGS_(VAR_ARGS_SIZE(__VA_ARGS__), what, __VA_ARGS__)

#define SEQUENCE_1(how)  how(1)
#define SEQUENCE_2(how)  SEQUENCE_1(how), how(2)
#define SEQUENCE_3(how)  SEQUENCE_2(how), how(3)
#define SEQUENCE_4(how)  SEQUENCE_3(how), how(4)
#define SEQUENCE_5(how)  SEQUENCE_4(how), how(5)
#define SEQUENCE_6(how)  SEQUENCE_5(how), how(6)
#define SEQUENCE_7(how)  SEQUENCE_6(how), how(7)
#define SEQUENCE_8(how)  SEQUENCE_7(how), how(8)
#define SEQUENCE_9(how)  SEQUENCE_8(how), how(9)
#define SEQUENCE_10(how) SEQUENCE_9(how), how(10)
#define SEQUENCE_11(how) SEQUENCE_10(how), how(11)
#define SEQUENCE_12(how) SEQUENCE_11(how), how(12)
#define SEQUENCE_13(how) SEQUENCE_12(how), how(13)
#define SEQUENCE_14(how) SEQUENCE_13(how), how(14)
#define SEQUENCE_15(how) SEQUENCE_14(how), how(15)
#define SEQUENCE_16(how) SEQUENCE_15(how), how(16)
#define SEQUENCE_17(how) SEQUENCE_16(how), how(17)
#define SEQUENCE_18(how) SEQUENCE_17(how), how(18)
#define SEQUENCE_19(how) SEQUENCE_18(how), how(19)
#define SEQUENCE_20(how) SEQUENCE_19(how), how(20)
#define SEQUENCE_21(how) SEQUENCE_20(how), how(21)
#define SEQUENCE_22(how) SEQUENCE_21(how), how(22)
#define SEQUENCE_23(how) SEQUENCE_22(how), how(23)
#define SEQUENCE_24(how) SEQUENCE_23(how), how(24)
#define SEQUENCE_25(how) SEQUENCE_24(how), how(25)
#define SEQUENCE_26(how) SEQUENCE_25(how), how(26)
#define SEQUENCE_27(how) SEQUENCE_26(how), how(27)
#define SEQUENCE_28(how) SEQUENCE_27(how), how(28)
#define SEQUENCE_29(how) SEQUENCE_28(how), how(29)
#define SEQUENCE_30(how) SEQUENCE_29(how), how(30)
#define SEQUENCE_31(how) SEQUENCE_30(how), how(31)
#define SEQUENCE_32(how) SEQUENCE_31(how), how(32)
#define SEQUENCE(how, N) SEQUENCE_##N(how)

#endif //!__RUNTIME_PREPROCESSOR_H__
