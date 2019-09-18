class QueryClass:
    def __init__(self):
        import mysql.connector as mariadb
        self.__mdbconn = mariadb.connect(host = 'db', user='pi', password='3_Yellow_mujis', database='iot')

    def addSensor(self, id, type, loc):
        self.__mdbconn.reset_session()
        cur.execute("INSERT INTO sensor VALUES (%d, %s, %s)", (id, type, loc))
        self.__mdbconn.commit()
        cur.close()
        return

    def addData(self, id, stamp, val, status):
        self.__mdbconn.reset_session()
        cur.execute("INSERT INTO data VALUES (%s, %d, %s, %s)", (id, stamp, val, status))
        self.__mdbconn.commit()
        cur.close()
        return

    def getLastStatus(self, id):
        self.__mdbconn.reset_session()
        cur = self.__mdbconn.cursor()
        cur.execute("SELECT status, timestmp FROM data WHERE id = %d ORDER BY timestmp DESC LIMIT 1", (id))
        statustime = cur.fetchall() #eates tuple list
        cur.close()
        return statustime

    def getLastValue(self, id):
        self.__mdbconn.reset_session()
        cur = self.__mdbconn.cursor()
        cur.execute("SELECT value, timestmp FROM data WHERE id = %d ORDER BY timestmp DESC LIMIT 1", (id))
        valuetime = cur.fetchall()
        cur.close()
        return valuetime

    def getLastSpecificStatus(self, id, status):
        self.__mdbconn.reset_session()
        cur = self.__mdbconn.cursor()
        cur.execute("SELECT status, timestmp FROM data WHERE id = %d AND status = %s ORDER BY timestmp DESC LIMIT 1", (id, status))
        statustime = cur.fetchall()
        cur.close()
        return statustime

    def deleteOldRows(self, time):
        import time
        import datetime
        self.__mdbconn.reset_session()
        cur = self.__mdbconn.cursor()
        cur.execute("DELETE FROM data WHERE ((data.timestmp - time).total_seconds > 129600)"")
        self.__mdbconn.commit()
        cur.close()
        return
