--INSTRUCTION FOR SETTING UP DB
-------------------------------
--do: Install MySql.
--username: root
--pwd: CloudMetrics1
--run: create database cloudmetrics;
--run: the following schema and seed data creation commands.

--------------------------------
--       Company
--------------------------------
CREATE TABLE Company (
  companyId int NOT NULL AUTO_INCREMENT,
  companyName varchar(200) NOT NULL,
  email varchar(120) NOT NULL UNIQUE,
  password varchar(80),
  isDirty int,

  PRIMARY KEY (companyId)
) AUTO_INCREMENT=1; 



