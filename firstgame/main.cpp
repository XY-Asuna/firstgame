#include <SFML/Graphics.hpp>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <thread>
#include <memory>
#include <iostream>
#include <mutex>
#include <sstream>
#include <optional>
#include <stdio.h>
#include <immintrin.h>
#include <chrono>
sf::RenderWindow window;
std::optional<sf::Event> start_event;
std::mutex mtx,grade_mtx;//lock
float lac = 1,bomb_speed;
int totlen,totlen_copy,grade,state,totbomb;
sf::CircleShape circle(20.f),snake[100006],apple,bomb[1000];
sf::CircleShape circle_copy(20.f), snake_copy[100006],apple_copy;
sf::Time frametime = sf::microseconds(1000),frametime_rend=sf::microseconds(16667),bomb_time_set=sf::seconds(10);
sf::Clock frameclock,frameclock_rend,bomb_time;
sf::RectangleShape boundary({ 780.f,580.f });
/*// sf::Font font;
// sf::Text text(font);//  需要能延迟初始化的。。
//这个不是 延迟的 吗？先引用font 后面再font导入字体 我之前一直这么写都没问题
//虽然是这样 但是其实可能有问题 不是好习惯 
// 甚至以前这些全局变量初始化顺序都没指定。。。
//ohno 他的text括号里面必须有东西 我没办法才引用font的*/
std::unique_ptr<sf::Font> font{};
std::unique_ptr<sf::Text> text{}, text_grade{}, start_text{}, end_text{};
/*// 傻逼vs 加载不出来就直接补全乱七八糟的东西

//fking函数没调用过 没字体用
//ptr是什么来的 pointer
//你要不试试看能不能用*/
void drawing_ing()
{
    window.draw(boundary);
    window.draw(*text);
    window.draw(apple_copy);
    for (int i = 1; i <= totlen_copy; i++) window.draw(snake_copy[i]);
    window.draw(circle_copy);
    window.draw(*text_grade);
    for(int i=1;i<=totbomb;i++) window.draw(bomb[i]);
}
void darwing_start()
{
    window.draw(*start_text);
}
void darwing_end()
{
    window.draw(*end_text);
}
void Copy()
{
    totlen_copy = totlen;
    for (int i = 1; i <= totlen_copy; i++) snake_copy[i] = snake[i];
    apple_copy = apple;
    circle_copy = circle;
}
void renderingThread(sf::RenderWindow* window)
{
    // activate the window's context
    window->setActive(true);
    // the rendering loop
    while (window->isOpen())
    {
        // draw...
        if (frametime_rend.asMilliseconds() > frameclock_rend.getElapsedTime().asMilliseconds()) continue;
        frameclock_rend.restart();
        mtx.lock();
        Copy();
        mtx.unlock();
        window->clear(sf::Color::White);
        if(state==0) darwing_start();
        if (state == 1)
        {
            mtx.lock();
            Copy();
            mtx.unlock();
            drawing_ing();
        }
        
        if(state==2) darwing_end();
        window->display();
    }
}/*
// 给你看看 unique pointer怎么实现
// 等会给你讲解
//ohno这是什么 这个真是看天书了

// 这个是模板
// 也就是说 这个东西有参数 这个参数是类型本身
// 就像你用vector 往尖括号里写的就是模板类型参数 eg vector<int> a;
// 然后你就发现vector神奇地适应了所有类型 既可以vector<int> 也可以vector<char> 确实
// 所以模板就是这个作用
// my_unique_ptr<int> => T = int
template<typename T>
struct my_unique_ptr {
    my_unique_ptr() = default; // 默认构造 不管它 好的
    T* storage{};               // 反正我置空了 //大概是没用的初始化 不管他

    template<typename... Args> // 这个三个点是可变参数包，表示一堆类型 //放入一些int char 之类的 反正是一堆
    my_unique_ptr(Args&&... args) { // 然后这个构造函数接受可变的参数 ...叫做展开运算符 这两个引用符号是因为有引用折叠 然后这个叫万能引用
        // 引用折叠也就是说 T && && = T && T & && = T & 
        // 用万能引用可以推导出包括引用的类型 如果不用万能引用就是推导出不带引用的类型了一般
        // 比如 int & 就叫包括引用的类型 int 就是没有引用的 //这个我大概知道
        storage = new T(std::forward<Args>(args)...);// ... 是展开前面这个表达式
        // 反正我们达到了目的 （）  用 new T(...) 就是分配并构造一个 T 返回一个 T* 正确无比
    }

    // 漏了星号运算符
    T &operator*() const noexcept {
        return *storage;
    }
    // 还漏了箭头
    T *operator->() const noexcept {
        return storage;
    } // sb vs 好像没错

    // 比如传入(9, std::string(), str), str 是一个 std::string 变量
    // 那么 Args 会推导成 (int(好吧我忘记这种数字字面量是什么 大概率是int//是int吧 小概率 int &&), std::string &&, std::string &)
    // 
    // template<typename T> func(T t) {...}  func(int) => T = int 假如不能自动推导 就要手动写 比如make_unique<T> 还有下面的fuck<T>(...)
    // my_unique_ptr(int, float) => Args = [int, float] 比如这个
    // 这个是模板函数
    // 有个例子是 sort
    // 你再一次神奇地发现sort能用在很多东西上
    // sort(数组),vector,什么什么的 
    // 它会自动推导
    // 那也是用了模板 不用写尖括号是因为通过参数自动推导出模板类型参数了 //哪个是用于自动推导的

    my_unique_ptr(const my_unique_ptr &) = delete;
    my_unique_ptr& operator=(const my_unique_ptr&) = delete;
    // 第一个叫复制构造函数 显然 unique ptr 不能复制 因为它表达了所有权
    // c++ 搞这些主要就是让你更好管理你的对象
    // rust 的所有权显然更严格 c++ 就要你自己手动搞这些了
    // 下面那个是运算符重载 复制构造赋值运算符 所以把这两个都 delete 了 不允许这个操作
    // 当你尝试复制unique_ptr 就会报错 看下面
    //这是把没用的delete
    
    my_unique_ptr(my_unique_ptr &&other) noexcept : storage(other.storage) {
        other.storage = nullptr;
    }
    my_unique_ptr& operator=(my_unique_ptr &&other) noexcept {
        storage = other.storage;
		other.storage = nullptr;
        return *this;
	}
    // 不能复制 那怎么传递 unique_ptr 呢
    // c++ 有个叫移动的东西，虽然rust也有 但是这两个不一样
    // rust 里被移动的对象就是已经被摧毁的状态 // 移动后摧毁原来的?//对 你可以想象它就是整个挪窝了 原来
    // c++ 移动之后 原来这个对象并没有“消失”或者摧毁 它说不定还能用 甚至一定要处于可以安全析构的状态
    // 上面两个就是移动构造函数 和对应的赋值符号 //把赋值（=）的作用稍作改变
    // 参数是右值才会调用这个移动 看下面
    // 可以看到就是把这个指针转移到新的 unique_ptr 里面 旧的要置空 让它处于可安全析构的状态
    
    //析构 == 摧毁？
    // 你说是也行
    // 这个就是析构 就是它走出属于它的 scope 之后，编译器会调用这个 //okok
    // 显然这样就能很方便实现一些清理逻辑
    // 那我们 unique_ptr 显然就是要释放这个玩意 所以直接 delete delete 空指针没事 所以不管他
    // 这个叫 RAII resource acquisition is initialization, 构造的时候完成初始化 然后自动出 scope 析构 释放里面的资源
    // 当然也有其他用法 比如有个std::lock_guard 传入一个mutex做构造参数 构造的时候上锁 析构的时候解锁
    ~my_unique_ptr() {
        delete storage;
    }
    //也就是 unique_ptr 可以自动推导参数 用完会自动释放
};

template<typename T, typename... Args>
auto fuck(Args&&... args) { // 这两个是为了能让Args推导出带引用的类型
    return T(std::forward<Args>(args)...);//这个forward是什么来的 
    // 然后这个Forward使用聪明的重载 把 args 按原来的类型（Args）返回//很聪明
    //也就是说 我给fuck一个参数 就会返回 T(这个参数)
}
// 主要和值类型有关系 反正比较猥琐 一般转发参数都会用这个 保证你fuck<T>(...) T(...), 接受的值是一样的 原来是左值就是传递左值 原来是右值就传递右值 调用到正确的函数
//okok
// 
// arguments 就是参数
//arg is what
*/
void setup_ur_fking_text() {
   /* //不好是指针不会用。
    // 太过智能le std::unique_ptr
    //不用指针怎么管理
    //没事是智能指针
    //他这个智能的意思是 他会在合适的时候释放掉里面的东西 所以不用手动delete
    // sf::Font(const std::filesystem::path &path)//固定的参数 看来不会自动转换 again
    // 确实 make_unique<T>(args) 就是转发参数到 T(args)
    //类似于 sf::Font font("dsaasdasd.otf");*/
    font = std::make_unique<sf::Font>(std::filesystem::path("SourceHanSerifCN-Medium.otf"));//这里是构造了一个Font类型的东东 
    //它这里怎么知道调用的是font.setFontfile
    //sf::Font f = fuck<sf::Font>(std::filesystem::path("SourceHanSerifCN-Medium.otf")); // 非常完美 就这个原理
    text = std::make_unique<sf::Text>(*font);
    
    /*//auto test1 = std::move(text); // std::move 把参数的类型转换为右值引用返回
    //auto test2(std::move(test1)); // 这个就调用了移动构造函数
    // 于是这里就调用了移动构造复制运算符 //聪明啊！ 听懂已经 那就是比较明了的
    // 除了 move 的作用比较微妙 还有移动构造不会摧毁原对象 基本上还是很明了
    // 好 于是你就发现这个规则还是非常清爽的 有原因的 
    // unique ptr 叫 unique 是有原因的 //醍醐灌顶 不错不错
    // 这样就不用管理共享的问题 并且可以非常简单的写释放掉它的逻辑
    //auto test = text;
    // make_unique 就是构造一个对象 然后make_unique参数就是构造函数的参数
    // 我看到 sf::Font 有一个构造函数接受路径
    // 然后返回指向这个对象的unique_ptr

    // my_unique_ptr<sf::Font> my_font(std::filesystem::path("SourceHanSerifCN-Medium.otf")); // 我草不行了明天再搞
    // auto my_test = std::move(my_font);
    // ok 非常好
    // 成功
    //没事你看
    // 并没有问题 因为没法复制（）
    // 只是这个报错怎么这么诡异（）
    //不行了大脑过载了 啥都听不懂了 没事 还剩下注释*/
	text->setFillColor(sf::Color::Black);
	circle.setFillColor(sf::Color(245,181,210));
	text->setFont(*font);
	text->setString(L"eat the red apple to extend.And press WASD to move.");
	text->setCharacterSize(24);
    sf::FloatRect rc = text->getLocalBounds();
    sf::Vector2f temp;
    temp.y = rc.position.length() / 2.0f, temp.x = rc.size.length() / 2.0f;
    text->setPosition(sf::Vector2f(800 / 2.0f, 200 / 2.0f));
    text->setOrigin(temp);
    
    
    text_grade = std::make_unique<sf::Text>(*font);
    text_grade->setFillColor(sf::Color::Black);
    text_grade->setFont(*font);
	text_grade->setCharacterSize(24);
	text_grade->setPosition(sf::Vector2f(20, 20));

    start_text = std::make_unique<sf::Text>(*font);
    start_text->setFillColor(sf::Color::Black);
    start_text->setFont(*font);
    start_text->setCharacterSize(24);
    start_text->setString(L"Welcome to the snake game! Press any key to start!");
    rc = start_text->getLocalBounds();
    temp.y = rc.position.length()/ 2.0f, temp.x = rc.size.length() / 2.0f;
    start_text->setPosition(sf::Vector2f(800 / 2.0f, 500 / 2.0f));
    start_text->setOrigin(temp);

    end_text = std::make_unique<sf::Text>(*font);
    end_text->setFillColor(sf::Color::Black);
    end_text->setFont(*font);
    end_text->setCharacterSize(24);

}
void end_text_setting()
{
    std::stringstream temp1;
    if(totlen<100000)
        temp1 << "You die! Press any key to restart!\nYour final grade is: " << grade;
    else
        temp1 << "You win! Press any key to restart!\nYour final grade is: " << grade;
    end_text->setString(temp1.str());
    sf::FloatRect rc = end_text->getLocalBounds();
    sf::Vector2f temp;
    temp.y = rc.position.length() / 2.0f, temp.x = rc.size.length() / 2.0f;
    end_text->setPosition(sf::Vector2f(800 / 2.0f, 500 / 2.0f));
    end_text->setOrigin(temp);
}
void moving()
{
    for (int i = totlen; i > 0; i--)
        snake[i].setPosition(snake[i - 1].getPosition());
    snake[0].setPosition(circle.getPosition());
}
void control()
{
    
    if (state == 1)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
            circle.move({ 0.f, -3.f*lac }),moving();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
            circle.move({ 0.f, 3.f*lac }),moving();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
            circle.move({ -3.f*lac, 0.f }),moving();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
            circle.move({ 3.f*lac, 0.f }),moving();
    }
}
int rx,ry;
void random()
{
    rx = rand() % 800;
    ry = rand() % 600;
}
void add()
{
    snake[++totlen].setPosition(snake[totlen].getPosition());
    snake[totlen].setRadius(20.f);
    snake[totlen].setFillColor(sf::Color(241, 158, 194));
    snake[totlen].setPosition(snake[totlen - 1].getPosition());
}

double getdistance(double a, double b, double x, double y)
{
    return sqrt((a - x) * (a - x) + (b - y) * (b - y));
}
bool check()
{
    for (int i = 1; i <= totlen; i++)
        if (getdistance(snake[i].getPosition().x, snake[i].getPosition().y, rx, ry) <= 40.0) return 0;
	if (rx <= 40 || ry <= 40 || rx >= 740 || ry >= 540) return 0;
    return 1;
}
bool check_eat()
{
    if (getdistance(circle.getPosition().x, circle.getPosition().y, apple.getPosition().x, apple.getPosition().y) <= 20.0) return 1;
    return 0;
}
void add_apple()
{
    random();
    while (!check()) random();
    sf::Vector2f temp;
    temp.x = rx, temp.y = ry;
    apple.setPosition(temp);
    apple.setFillColor(sf::Color::Red);
}
bool check_boundary()
{
    sf::Vector2f temp=circle.getPosition();
    if (temp.x <= 10 || temp.x >= 750 || temp.y <= 10 || temp.y >= 550) return 0;
    return 1;
}
void grade_to_str()
{
    std::stringstream grade_str;
    grade_str << "Your grade: "<<grade;
    text_grade->setString(grade_str.str());
}
int check_head()
{
       if (rx <= 10 || rx >= 700 || ry <= 10 || ry >= 500) return 0;
       return 1;
}
void bomb_add()
{
    bomb[++totbomb].setFillColor(sf::Color::Black);
    sf::Vector2f temp;
    temp.x = 760.f, temp.y = 560.f;
    bomb[totbomb].setPosition(temp);
    bomb[totbomb].setRadius(15.f);
    bomb[totbomb].setFillColor(sf::Color::Black);
    bomb[totbomb].setOutlineColor(sf::Color::Blue);
    bomb[totbomb].setOutlineThickness(5.f);
}
void start_setting()
{
	totlen = 0;
    snake[0] = circle;
    apple.setRadius(20.f);
    sf::Vector2f temp;
    random();
    while (!check_head()) random();
    temp.x = rx, temp.y = ry;
    //temp.x = 40, temp.y = 40;
	circle.setPosition(temp);
    boundary.setOutlineColor(sf::Color::Black);
	boundary.setOutlineThickness(10.f);
    temp.x = 10, temp.y = 10;
    boundary.setPosition(temp);
    add_apple();
    grade = 0;
    grade_mtx.lock();
    grade_to_str();
    grade_mtx.unlock();
    totbomb = 0;
    //bomb_add();
    bomb_speed = 1;
}
void bomb_move()
{
    sf::Vector2f temp_bomb,temp_head;
    for(int i=1;i<=totbomb;i++)
    {
        temp_bomb = bomb[i].getPosition(), temp_head = circle.getPosition();
        double dis = getdistance(temp_bomb.x, temp_bomb.y, temp_head.x, temp_head.y);
        double sn=temp_bomb.y-temp_head.y,cs=temp_bomb.x-temp_head.x;
        temp_bomb.x -= bomb_speed*0.5*cs / dis;
        temp_bomb.y -= bomb_speed*0.5*sn / dis;
        bomb[i].setPosition(temp_bomb);
    }
}
int check_bomb()
{
    for(int i=1;i<=totbomb;i++)
        if (getdistance(bomb[i].getPosition().x, bomb[i].getPosition().y, circle.getPosition().x, circle.getPosition().y) <= 40.0) return 0;
    return 1;
}
int main()
{
    srand(time(0));
    // create the window
    window.create(sf::VideoMode({ 800, 600 }), "My window");
     // Throws sf::Exception if an error occurs
    // run the program as long as the window is open
    setup_ur_fking_text();
    start_setting();//这里要改成那个fking的
    state = 0;
    frameclock.restart();
    window.setActive(false);
    std::thread thread(&renderingThread, &window);
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        while (const std::optional event = window.pollEvent())
        {
            // "close requested" event: we close the window
            if (event->is<sf::Event::Closed>())
                window.close();

        }
        if (frametime.asMilliseconds() > frameclock.getElapsedTime().asMilliseconds())
        {
            int sleep_time = frametime.asMicroseconds() - frameclock.getElapsedTime().asMicroseconds();
            std::this_thread::sleep_for(std::chrono::microseconds(sleep_time));
        }
        frameclock.restart();

        if (state == 0 || state == 2)
        {
			const auto start_event = window.pollEvent();
            if (start_event.has_value() && start_event->is<sf::Event::KeyPressed>()/* || start_event->is<sf::Event::KeyReleased>()*/)
            {
                state = 1;
                bomb_add();
                bomb_time.restart();
            }
        }

        mtx.lock();
        control();
        if (state == 1)
        {
            if (check_eat())
            {
                ++grade;
                grade_to_str();
                add_apple();
                for(int i=1;i<=5;i++) 
                    add();
            }
            bomb_move();
            if (!check_boundary() || !check_bomb() || totlen>=100000)
            {
                state = 2;
                end_text_setting();
                start_setting();
            }
            if (bomb_time_set.asSeconds() <= bomb_time.getElapsedTime().asSeconds())
            {
                bomb_add();
                bomb_speed+=0.5;
                bomb_time.restart();
            }
        }
        mtx.unlock();
    }
    thread.join();
}
