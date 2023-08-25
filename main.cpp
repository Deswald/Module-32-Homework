#include <iostream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

class DBConnection {
public:
    DBConnection(std::string ip, uint16_t port) : _ip_DB(ip), _port_DB(port) {}
    virtual ~DBConnection() {}
    virtual bool open() = 0 {}
    virtual bool close() = 0 {}
    virtual void execQuery() = 0 {}
protected:
    std::string _ip_DB;
    uint16_t _port_DB;
};

class ClassThatUsesDB : public DBConnection {
public:
    ClassThatUsesDB(std::string ip, uint16_t port) : DBConnection(ip, port) {}
    ~ClassThatUsesDB() {}

    virtual bool open() override {
        if (!_ip_DB.empty() && _port_DB)
        {
            std::cout << "Connection to address " << _ip_DB << ":" << _port_DB << " established" << std::endl;
            return true;
        }
        return false;
    }
    virtual bool close() override {
        if (!_ip_DB.empty() && _port_DB)
        {
            _ip_DB.clear();
            _port_DB = 0;
            std::cout << "Connection closed" << std::endl;
            return true;
        }
        std::cout << "The connection was closed but had an invalid IP or PORT" << std::endl;
        return true;
    }
    virtual void execQuery() override {
        if (!_ip_DB.empty() && _port_DB)
        {
            std::cout << "Request sent to address " << _ip_DB << ":" << _port_DB << std::endl;
            return;
        }
        std::cout << "ERROR: Wrong IP or PORT" << std::endl;
        return;
    }
};

void testDB(DBConnection* test)
{
    test->open();
    test->execQuery();
    test->close();
}

class MockDBConnection : public DBConnection {
public:
    MockDBConnection(std::string ip, uint16_t port) : DBConnection(ip, port) {}

    MOCK_METHOD(bool, open, (), (override));
    MOCK_METHOD(void, execQuery, (), (override));
    MOCK_METHOD(bool, close, (), (override));
};

TEST(DBCTest, test1)
{
    ClassThatUsesDB test_object1("127.0.0.1", 55555);
    EXPECT_EQ(test_object1.open(), true);
    test_object1.execQuery();
    EXPECT_EQ(test_object1.close(), true);
}

TEST(DBCTest, test2)
{
    ClassThatUsesDB test_object2("127.0.0.1", 0);

    EXPECT_EQ(test_object2.open(), false);
    test_object2.execQuery();
    EXPECT_EQ(test_object2.close(), true);
}

TEST(MockDBConnection, test3)
{
    MockDBConnection test_object3("127.0.0.1", 0);

    EXPECT_CALL(test_object3, open).Times(1);
    EXPECT_CALL(test_object3, close).Times(1);
    EXPECT_CALL(test_object3, execQuery).Times(1);

    testDB(&test_object3);
}

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}