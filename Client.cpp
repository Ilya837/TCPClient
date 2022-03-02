#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>



void sync_echo(std::string msg)
{
    msg.append("\n");
    //������ '����������� �����'
    boost::asio::io_service io_server;
    //������ ����� (�����/�������)
    boost::asio::ip::tcp::socket sock(io_server);
    // ������ ������, �� �������� ����� ����������
    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address_v4::from_string("127.0.0.1"), 9999);
    //�������� ����������� � ��������. ��� ������� ����� ������. ��� ����� ���������� ����������.
    sock.connect(ep);
    //���������� ������� ���������
    sock.write_some(boost::asio::buffer(msg));

    boost::asio::streambuf buf;
    //��������� ����� �������, � �������� ��������� �����, � �����. ����� ��������� ������ ������ � ������
    int bytes = read_until(sock, buf, "\n");
 
    if (bytes == 0) {
        std::cout << "no message" << std::endl;
        return;
    }

    //�������� ����� � ������, ������� ������ ������� � ������� � ������� �����-�� �� ��� � ������������ ��������� ��� ���
    std::ostringstream copy;
    copy << &buf;
    std::cout << "server echoed our " << msg << ": " << (copy.str() == msg ? "GOOD" : "FAIL") << std::endl;
    // �������� ����� � ��������
    sock.close();

}

int main(int argc, char* argv[])
{
    std::string messages[4] = { "John says hi", "so does James", "Lucy just got home", "Boost.Asio is Fun!" };
    //������ ������� �������� bind-��*
    boost::thread_group threads;

    //��������� "������" �� ��������� �������
    for (auto& message : messages) {
        //��������� � ������� bind, ������� ����� ����� �����������
        threads.create_thread(boost::bind(sync_echo, message));

        //���������� ��������� 1000 �� �� ������� ���������� bind-� �� ������� 
        boost::this_thread::sleep(boost::posix_time::millisec(1000));
    }

    //��������� ��������� bind-� �� �������, ���� ��� , ����������� � ����� �������, �� �����������
    threads.join_all();
    getchar();
}

/*
*bind - ��� ����������� � ���� ������ �������/������, ������� ����� ���������, � ���������� ���� �������/������.
*/