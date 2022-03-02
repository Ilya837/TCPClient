#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>



void sync_echo(std::string msg)
{
    msg.append("\n");
    //Создаём 'Планировщик задач'
    boost::asio::io_service io_server;
    //Создаём сокет (вилку/разетку)
    boost::asio::ip::tcp::socket sock(io_server);
    // создаём адресс, по которому будем обращаться
    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address_v4::from_string("127.0.0.1"), 9999);
    //Пытаемся соединиться с сервером. При неудаче выдаёт ошибку. При удаче продолжает выполнение.
    sock.connect(ep);
    //Отправляем серверу сообщение
    sock.write_some(boost::asio::buffer(msg));

    boost::asio::streambuf buf;
    //Считываем ответ сервера, к которому подключен сокет, в буфер. Метод возращает размер ответа в байтах
    int bytes = read_until(sock, buf, "\n");
 
    if (bytes == 0) {
        std::cout << "no message" << std::endl;
        return;
    }

    //копируем буфер в объект, который сможем вывести в консоль и выводим такой-же он как и отправленное сообщение или нет
    std::ostringstream copy;
    copy << &buf;
    std::cout << "server echoed our " << msg << ": " << (copy.str() == msg ? "GOOD" : "FAIL") << std::endl;
    // Обрываем связь с сервером
    sock.close();

}

int main(int argc, char* argv[])
{
    std::string messages[4] = { "John says hi", "so does James", "Lucy just got home", "Boost.Asio is Fun!" };
    //Создаём очередь запусков bind-ов*
    boost::thread_group threads;

    //Запускаем "Проход" по элементам массива
    for (auto& message : messages) {
        //Добавляем в очередь bind, который начнёт сразу выполняться
        threads.create_thread(boost::bind(sync_echo, message));

        //Заставляем подождать 1000 мс до запуска следующего bind-а из очереди 
        boost::this_thread::sleep(boost::posix_time::millisec(1000));
    }

    //Запрещаем выполнять bind-ы из очереди, пока все , добавленные к этому моменту, не завершаться
    threads.join_all();
    getchar();
}

/*
*bind - это объединение в один объект функции/метода, который нужно запустить, и параметров этой функции/метода.
*/