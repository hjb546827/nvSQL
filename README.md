# 微型数据库管理系统

## 简介

采用宿主操作系统的文件系统，对标MySQL的微型数据库管理系统。

## 运行环境

- Ubuntu 23.04+
- gcc13+

## 编译

``` bash
cd nvSQL
make
```

## 运行

```bash
bin/nvSQL
```

## 项目结构

```html
nvSQL
  |---- data // 数据文件
  |---- bin // 可执行文件
  |---- src // 源代码
  |       |---- bpTree // b+树结构
  |                    |---- bpTree.h // b+树
  |                    |---- dataMgr.h // 磁盘交互
  |                    +---- type_traits.h // type_traits
  |       |---- DB.h // DB类
  |       |---- DDL.cpp // DDL语句实现
  |       |---- DML.cpp // DML语句实现
  |       |---- DQL.cpp // DQL语句实现
  |       |---- SQL.h // DDL、DML、DQL语句声明
  |       |---- main.cpp // 程序入口
  |       |---- tData.h // 表对象中行结构与列结构定义
  |       |---- table.h // 表对象
  |       +---- utility.h // 全局变量、全局函数、计时器等
  +---- makefile // makefile文件
```

## 系统功能

### 具体功能

- 存储功能

  - 目录结构
  ```html
  data // 数据文件
  |---- person // 数据库名
  |          |-- person.dat      // 表的数据文件
  |          |-- person.ind      // 同名表的索引文件
  |          +-- person.prof    // 同名表的配置文件
  +---- other
  ```
- DDL
  - create database
    功能：创建数据库
    语法：create database <dbname>;
  - drop database
    功能：删除数据库
    语法：drop database <dbname>;
  - use
    功能：切换数据库
    语法：use <dbname>;
  - create table
    功能：创建表
    语法：create table <table-name> (
    				<column> <type> [ primary ],
    				...);
  - drop table
    功能：删除表及其索引
    语法：drop table <table-name>；
- DML
  - delete
    功能：根据条件（如果有）删除表中的记录。
    语法：delete <table> [ where <cond> ];
    			其中，<column>： <column-name> |\*。一个或多个列名，中间用逗号隔开。\*表示所有列。
    			where 子句：可选。如无，表示无条件查询。字符串数据用双引号括起来。下同。
    				<cond> ：<column> <op> <const-value>
    				<op>：=、<、>、<=、>= 之一
  - insert
    功能：在表中插入数据。
    语法：insert <table> values (<const-value>[, <const-value>…]);
  - update
    功能：根据条件（如果有）更新表中的记录。如无条件，则更新整张表。
    语法：update <table> set <column> = <const-value> [ where <cond> ];
- DQL
  - select
    功能：根据条件（如果有）查询表，显示查询结果。
    语法：select <column> from <table> [ where <cond> ]；
- 索引
  使用b+树建立索引，默认建立在表的主键上

### 界面

```html
db> create database test;
Create database successfully in 11.091ms!
test> create table student(id int, name string, age int, tel string primary, score int);
Create table successfully in 34.486ms!
test> insert student values(0, "sam", 16, "12345678", 80);
Insert table successfully in 52.191ms!
test> select name, score, tel from student;
+------+-------+----------+
| name | score | tel      |
+------+-------+----------+
| sam  | 80    | 12345678 |
+------+-------+----------+
Select record successfully in 27.569ms!
test> update student set score=0 where name = "sam";
Update record successfully in 51.525ms!
test> select * from student;
+----+------+-----+----------+-------+
| id | name | age | tel      | score |
+----+------+-----+----------+-------+
| 0  | sam  | 16  | 12345678 | 0     |
+----+------+-----+----------+-------+
Select record successfully in 26.86ms!
test> delete student where tel = "12345678";
Delete record successfully in 35.903ms!
test> drop database test;
Completely drop the database? (y/n)
y
Drop database successfully in 14.454ms!
db> exit
Bye
```

## 测试用例

见testcase.txt
