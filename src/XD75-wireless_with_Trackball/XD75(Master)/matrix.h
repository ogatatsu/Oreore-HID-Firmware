#include "MatrixScan.h"

using namespace hidpg;

// マトリックス回路で使うピンの定義
constexpr uint8_t OUT0 = 3;   // COL0
constexpr uint8_t OUT1 = 2;   // COL1
constexpr uint8_t OUT2 = 4;   // COL2
constexpr uint8_t OUT3 = 12;  // COL3
constexpr uint8_t OUT4 = 11;  // COL4
constexpr uint8_t OUT5 = 18;  // COL5
constexpr uint8_t OUT6 = 24;  // COL6
constexpr uint8_t OUT7 = 23;  // COL7
constexpr uint8_t OUT8 = 7;   // COL8
constexpr uint8_t OUT9 = 26;  // COL9
constexpr uint8_t OUT10 = 25; // COL10
constexpr uint8_t OUT11 = 27; // COL11
constexpr uint8_t OUT12 = 30; // COL12
constexpr uint8_t OUT13 = 29; // COL13
constexpr uint8_t OUT14 = 28; // COL14
constexpr uint8_t outPins[] = {OUT0, OUT1, OUT2, OUT3, OUT4, OUT5, OUT6, OUT7, OUT8, OUT9, OUT10, OUT11, OUT12, OUT13, OUT14};

constexpr uint8_t IN0 = 16; // ROW0
constexpr uint8_t IN1 = 31; // ROW1
constexpr uint8_t IN2 = 13; // ROW2
constexpr uint8_t IN3 = 15; // ROW3
constexpr uint8_t IN4 = 5;  // ROW4
constexpr uint8_t inPins[] = {IN0, IN1, IN2, IN3, IN4};

constexpr uint8_t outLength = sizeof(outPins) / sizeof(outPins[0]);
constexpr uint8_t inLength = sizeof(inPins) / sizeof(inPins[0]);

// スイッチとマトリックスの定義
// IN0
Switch sw1(1);
Switch sw6(6);
Switch sw11(11);
Switch sw16(16);
Switch sw21(21);
Switch sw26(26);
Switch sw31(31);
Switch sw36(36);
Switch sw41(41);
Switch sw46(46);
Switch sw51(51);
Switch sw56(56);
Switch sw61(61);
Switch sw66(66);
Switch sw71(71);
// IN1
Switch sw2(2);
Switch sw7(7);
Switch sw12(12);
Switch sw17(17);
Switch sw22(22);
Switch sw27(27);
Switch sw32(32);
Switch sw37(37);
Switch sw42(42);
Switch sw47(47);
Switch sw52(52);
Switch sw57(57);
Switch sw62(62);
Switch sw67(67);
Switch sw72(72);
// IN2
Switch sw3(3);
Switch sw8(8);
Switch sw13(13);
Switch sw18(18);
Switch sw23(23);
Switch sw28(28);
Switch sw33(33);
Switch sw38(38);
Switch sw43(43);
Switch sw48(48);
Switch sw53(53);
Switch sw58(58);
Switch sw63(63);
Switch sw68(68);
Switch sw73(73);
// IN3
Switch sw4(4);
Switch sw9(9);
Switch sw14(14);
Switch sw19(19);
Switch sw24(24);
Switch sw29(29);
Switch sw34(34);
Switch sw39(39);
Switch sw44(44);
Switch sw49(49);
Switch sw54(54);
Switch sw59(59);
Switch sw64(64);
Switch sw69(69);
Switch sw74(74);
// IN4
Switch sw5(5);
Switch sw10(10);
Switch sw15(15);
Switch sw20(20);
Switch sw25(25);
Switch sw30(30);
Switch sw35(35);
Switch sw40(40);
Switch sw45(45);
Switch sw50(50);
Switch sw55(55);
Switch sw60(60);
Switch sw65(65);
Switch sw70(70);
Switch sw75(75);

Switch *matrix[outLength][inLength] = {
    {&sw1, &sw2, &sw3, &sw4, &sw5},
    {&sw6, &sw7, &sw8, &sw9, &sw10},
    {&sw11, &sw12, &sw13, &sw14, &sw15},
    {&sw16, &sw17, &sw18, &sw19, &sw20},
    {&sw21, &sw22, &sw23, &sw24, &sw25},
    {&sw26, &sw27, &sw28, &sw29, &sw30},
    {&sw31, &sw32, &sw33, &sw34, &sw35},
    {&sw36, &sw37, &sw38, &sw39, &sw40},
    {&sw41, &sw42, &sw43, &sw44, &sw45},
    {&sw46, &sw47, &sw48, &sw49, &sw50},
    {&sw51, &sw52, &sw53, &sw54, &sw55},
    {&sw56, &sw57, &sw58, &sw59, &sw60},
    {&sw61, &sw62, &sw63, &sw64, &sw65},
    {&sw66, &sw67, &sw68, &sw69, &sw70},
    {&sw71, &sw72, &sw73, &sw74, &sw75},
};
