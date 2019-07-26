/*
Navicat SQLite Data Transfer

Source Server         : data.db
Source Server Version : 30706
Source Host           : :0

Target Server Type    : SQLite
Target Server Version : 30706
File Encoding         : 65001

Date: 2019-07-26 20:54:20
*/

PRAGMA foreign_keys = OFF;

-- ----------------------------
-- Table structure for "main"."futureWeather"
-- ----------------------------
DROP TABLE "main"."futureWeather";
CREATE TABLE "futureWeather" (
"days"  char,
"citynm"  char,
"weather"  char,
"wind"  char,
"winp"  char,
"temp_high"  cahr,
"temp_low"  char,
PRIMARY KEY ("days")
);

