DROP TABLE IF EXISTS sensor;
DROP TABLE IF EXISTS data;

CREATE TABLE sensor (
  id INT,
  type VARCHAR(20),
  location VARCHAR(20),
  PRIMARY KEY (id)
);

CREATE TABLE data (
  id INT,
  timestmp TIMESTAMP,
  value INT,
  status VARCHAR(20)
  PRIMARY KEY (id, timestmp)
);
