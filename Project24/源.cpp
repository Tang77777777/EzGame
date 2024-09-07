//享元模式优化版
#include <algorithm>
#include <bitset>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <graphics.h> // 引用 EasyX 绘图库头文件
#include <iostream>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

bool is_game_started = false;//标识当前游戏是否已经开始

bool running = true;

const int WINDOW_WIDTH = 1280;

const int WINDOW_HEIGHT = 720;

const int BUTTON_WIDTH = 192;

const int BUTTON_HEIGHT = 75;
class Atlas//动画所使用的图集(享元模式优化）
{
public:
    Atlas(LPCTSTR path, int num)
    {
        TCHAR path_file[256];
        for (int i = 0; i < num; i++)
        {

            _stprintf_s(path_file, path, i);

            IMAGE* frame = new IMAGE();//因为使用了new所以需要释放掉
            //                           一个malloc 对应一个free ，一个new 应该对应一个 delete
            loadimage(frame, path_file);

            mFrameList.push_back(frame);//将指针输入到容器中
        }
    }//check Done

    ~Atlas()
    {
        for (auto frame : mFrameList)//释放内存
        {
            delete frame;
        }
    }//check Done

public:
    std::vector<IMAGE*> mFrameList;
};

//动画封装类

class Animation//使用封装的逻辑使得代码更加简洁

{
    int mFrameIndex = 0;//动画帧索引

    int mTimer = 0;//动画计时器 使用计时器更符合我们需要满足动画播放稳定性的需求

    int mIntervalMs = 0;

    std::vector<IMAGE*> mFrameList; 

public://此时已成为公共资产，不能释放Atlas的指针
    Animation(Atlas* atlas , int interval)//文件所在的路径  当前动画使用的图片数量
    {
        anim_atlas = atlas;

        mIntervalMs = interval;
    }

    ~Animation() = default;

    //check fine

    void Play(int x, int y, int delta)//动画播放 渲染参数的位置 ，以及获取了多久
    {

        mTimer += delta;

        if (mTimer >= mIntervalMs)//定时器到达帧间隔，那么就能切换动画图片到下一帧
        {
            mFrameIndex = (mFrameIndex + 1) % anim_atlas -> mFrameList.size();

            mTimer = 0; //并重置定时器的值
        }

        DrawImage(x, y, anim_atlas -> mFrameList[mFrameIndex]); //通过上面的函数绘制当前的动画帧
    }
    //check fine
    //绘制透明图像

    // https://learn.microsoft.com/zh-cn/windows/win32/api/wingdi/nf-wingdi-alphablend?redirectedfrom=MSDN

    static void DrawImage(int x, int y, IMAGE* img) //绘制透明化的函数,即去除素材黑框
    {

        int w = img->getwidth();

        int h = img->getheight();

        AlphaBlend(GetImageHDC(NULL), x, y, w, h,

            GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA });
    }
private:
    Atlas* anim_atlas;
};
//公用的对象 将指针定义为全局变量

Atlas* atlas_player_left;
Atlas* atlas_player_right;
Atlas* atlas_enemy_left;
Atlas* atlas_enemy_right;
#pragma once
//动画实现
//Animation anim_left_player(_T("img/player_left_%d.png"), 6, 45);
//Animation anim_right_player(_T("img/player_right_%d.png"), 6, 45);
//玩家类

class Player//玩家类
{

    bool mIsMoveLeft = false;

    bool mIsMoveRight = false;

    bool mIsMoveUp = false;

    bool mIsMoveDown = false;

    int PLAYER_SPEED = 3; //玩家动画总帧数

    POINT mPos = { 720, 360 }; //玩家位置

    //在Draw实现阴影位置的定位
    IMAGE mImgShadow; //阴影图像

    Animation* mAnimLeft; //向左动作帧序列

    Animation* mAnimRight; //向右动作帧序列

public:

    const int FRAME_WIDTH = 80; //玩家图像宽度

    const int FRAME_HEIGHT = 80; //玩家图像高度

    const int SHADOW_WIDTH = 32; //玩家阴影图像宽度

    Player()

    {

        loadimage(&mImgShadow, _T("img/shadow_player.png"));

        mAnimLeft = new Animation(atlas_player_left , 45); //动画一共由6张图片构成，帧间隔45毫秒

        mAnimRight = new Animation(atlas_player_right, 45);
    }

    ~Player()

    {

        delete mAnimLeft;

        delete mAnimRight;
    }

    void ProcessEvent(const ExMessage& msg)

    {

        if (msg.message == WM_KEYDOWN)//实现主角移动功能
        {                              //当按键按下时

            switch (msg.vkcode)

            {

            case VK_UP:

                mIsMoveUp = true;

                break;

            case VK_DOWN:

                mIsMoveDown = true;

                break;

            case VK_LEFT:

                mIsMoveLeft = true;

                break;

            case VK_RIGHT:

                mIsMoveRight = true;

                break;
            }
        }

        else if (msg.message == WM_KEYUP)
        {//当按键抬起时

            switch (msg.vkcode)

            {

            case VK_UP:

                mIsMoveUp = false;

                break;

            case VK_DOWN:

                mIsMoveDown = false;

                break;

            case VK_LEFT:

                mIsMoveLeft = false;

                break;

            case VK_RIGHT:

                mIsMoveRight = false;

                break;
            }
        }
    }

    void Move()//移动逻辑
    {

        int dirX = mIsMoveRight - mIsMoveLeft;//x轴距离

        int dirY = mIsMoveDown - mIsMoveUp;//y轴距离

        double dirLen = sqrt(dirX * dirX + dirY * dirY);//直线距离

        if (dirLen != 0)
        {

            double normalizedX = dirX / dirLen;

            double normalizedY = dirY / dirLen;

            mPos.x += (int)(PLAYER_SPEED * normalizedX);

            mPos.y += (int)(PLAYER_SPEED * normalizedY);
        }
        //边界限制，使得玩家仅能在窗口内运动
        mPos.x = mPos.x < 0 ? 0 : mPos.x;

        mPos.x = mPos.x > 1200 ? 1200 : mPos.x;

        mPos.y = mPos.y < 0 ? 0 : mPos.y;

        mPos.y = mPos.y > 640 ? 640 : mPos.y;
    } // x

    void Draw(int delta)

    {

        int shadowPosX = mPos.x + (FRAME_WIDTH / 2 - SHADOW_WIDTH / 2);//使得阴影图片位置居中

        int shadowPosY = mPos.y + FRAME_HEIGHT - 8;//阴影放置于y轴的位置

        Animation::DrawImage(shadowPosX, shadowPosY, &mImgShadow);
        //实现左右移动时动画翻转
        static bool facingLeft = false;//定义一个私有变量，确定是否处于向左

        int dirX = mIsMoveRight - mIsMoveLeft;

        if (dirX < 0)//如果x轴上的移动方向小于0

            facingLeft = true;

        else if (dirX > 0)

            facingLeft = false;

        if (facingLeft)//根据其值判断向左或者向右的动画

            mAnimLeft->Play(mPos.x, mPos.y, delta);

        else

            mAnimRight->Play(mPos.x, mPos.y, delta);
    }

    const POINT& GetPos() const//用它返回玩家当前位置

    {

        return mPos;
    }
};

#pragma once
//子弹类

class Bullet//子弹类
{
public:
    POINT mPos = { 0 , 0 };
public:
    Bullet() = default;
    ~Bullet() = default;

    void Draw() const//颜色形状
    {
        setlinecolor(RGB(255, 155, 50));
        setfillcolor(RGB(255, 155, 50));
        fillcircle(mPos.x, mPos.y, RADIUS);
    }
private:
    const int RADIUS = 10;
};
//敌人类

class Enemy//敌人 类

{

    const int SPEED = 2; //移动速度

    const int FRAME_WIDTH = 80; //帧宽

    const int FRAME_HEIGHT = 80; //帧高

    const int SHADOW_WIDTH = 48; //阴影宽

    IMAGE mImgShadow; //阴影图像

    Animation* mAnimLeft; //向左动作帧序列

    Animation* mAnimRight; //向右动作帧序列

    POINT mPos = { 0, 0 }; //位置

    bool mFacingLeft = false; //朝左运动标志位

    bool mIsAlive = true;//是否活着

    //no check
public:
    Enemy()//敌人产生逻辑
    {

        loadimage(&mImgShadow, _T("img/shadow_enemy.png"));

        mAnimLeft = new Animation(atlas_enemy_left, 45);

        mAnimRight = new Animation(atlas_enemy_right, 45);

        enum class SpawnEdge
        {

            Up = 0,

            Down,

            Left,

            Right
        };

        SpawnEdge edge = (SpawnEdge)(rand() % 4);//使用随机产生
        //敌人上下左右的随机产生逻辑
        switch (edge)
        {
        case SpawnEdge::Up:

            mPos.x = rand() % 1280;

            mPos.y = -FRAME_HEIGHT;

            break;

        case SpawnEdge::Down:

            mPos.x = rand() % 1280;

            mPos.y = 720;

            break;

        case SpawnEdge::Left:

            mPos.x = -FRAME_WIDTH;

            mPos.y = rand() % 720;

            break;

        case SpawnEdge::Right:

            mPos.x = 1280;

            mPos.y = rand() % 720;

            break;

        default:

            break;
        }
    }

    ~Enemy()
    {
        delete mAnimLeft;

        delete mAnimRight;
    }

    bool CheckBulletCollision(const Bullet& bullet)//子弹碰撞检测
    {                         //添加了引用，类似于使用指针传参数，避免了对传入对象进行了不必要的拷贝构造//使用const避免
                              //在函数内部不小心对参数进行了修改
        bool BetweenX = (bullet.mPos.x > mPos.x && bullet.mPos.x < (mPos.x + FRAME_WIDTH)) ? true : false;
        //判断边界，上下左右两边
        bool BetweenY = (bullet.mPos.y > mPos.y && bullet.mPos.y < (mPos.y + FRAME_HEIGHT)) ? true : false;

        return BetweenX && BetweenY;
    }

    bool CheckPlayerCollision(const Player& player)//玩家碰撞检测
    {
        //将敌人的中心位置等效为点，判断点是否在玩家矩形碰撞箱内
        bool BetweenX = ((mPos.x + FRAME_WIDTH / 2) > player.GetPos().x
            && (mPos.x + FRAME_WIDTH / 2) < (player.GetPos().x + FRAME_WIDTH)) ? true : false;

        bool BetweenY = ((mPos.y + FRAME_HEIGHT / 2) > player.GetPos().y
            && (mPos.y + FRAME_HEIGHT / 2) < (player.GetPos().y + FRAME_HEIGHT)) ? true : false;

        return BetweenX && BetweenY;
    }

    void Move(const Player& player)//始终追踪玩家的移动
    {
        const POINT& playerPos = player.GetPos();

        int dirX = playerPos.x - mPos.x;//与玩家x轴距离

        int dirY = playerPos.y - mPos.y;//Y轴

        double dirLen = sqrt(dirX * dirX + dirY * dirY); //与玩家距离

        if (dirLen != 0)
        {

            double normalizedX = dirX / dirLen; // X轴方向

            double normalizedY = dirY / dirLen; // Y轴方向

            mPos.x += (int)(SPEED * normalizedX);

            mPos.y += (int)(SPEED * normalizedY);
        }

        if (dirX < 0)//判断
        {
            mFacingLeft = true;
        }
        else if (dirX > 0)
        {
            mFacingLeft = false;
        }
    }

    void Draw(int delta)//画面预载
    {

        int shadowPosX = mPos.x + (FRAME_WIDTH / 2 - SHADOW_WIDTH / 2);

        int shadowPosY = mPos.y + FRAME_HEIGHT - 35;

        Animation::DrawImage(shadowPosX, shadowPosY, &mImgShadow);

        if (mFacingLeft)

            mAnimLeft->Play(mPos.x, mPos.y, delta);

        else

            mAnimRight->Play(mPos.x, mPos.y, delta);
    }

    const POINT& GetPos() const//取得位置
    {

        return mPos;
    }
    //检测敌人是否或者
    void Hurt()//受击方法
    {
        mIsAlive = false;//被子弹击中即死
    }

    bool CheckAlive()
    {
        return mIsAlive;
    }

};

#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "Msimg32.lib")

//生成新的敌人
void TryGenerateEnemy(std::vector<Enemy*>& enemyList)//敌人生成
{
    const int INTERVAL = 100;
    //内置计数器，在固定时间内新增敌人
    static int counter = 0;

    if ((++counter) % INTERVAL == 0)
    {
        enemyList.push_back(new Enemy());
    }   
}

//子弹运行逻辑实现
void UpdateBullet(const Player& player, std::vector<Bullet>& bullets)//用以在住循环中实现子弹实时跟随主角的逻辑
{//让子弹拥有做圆周运动的切向速度外，还有一个不断波动的径向速度
    const double RADIAL_SPEED = 0.0045; // 径向波动速度 决定了子弹距离玩家时远时近的波动速度

    const double TANGENT_SPEED = 0.0055;// 切向波动速度 决定了圆周速度的快慢

    double radian_interval = 2 * 3.14159 / bullets.size(); // 子弹之间的弧度间隔 使用360度除以子弹的个数
    //遍历子弹列表中的每一个子弹
    double radius = 100 + 25 * sin(GetTickCount() * RADIAL_SPEED);//子弹旋转的实现 按照时间增加递增GetTickCount * speed即角度即可

    for (size_t i = 0; i < bullets.size(); i++)
    {
        double radian = GetTickCount() * TANGENT_SPEED + radian_interval * i;// 当前子弹所在弧度值

        bullets[i].mPos.x = player.GetPos().x + player.FRAME_WIDTH / 2 + (int)(radius * sin(radian));

        bullets[i].mPos.y = player.GetPos().y + player.FRAME_HEIGHT / 2 + (int)(radius * cos(radian));
    }

}

//绘制分数
void DrawPlayerScore(int score)
{
    static TCHAR text[64];
    _stprintf_s(text, _T("当前玩家得分： %d"), score);

    setbkmode(TRANSPARENT);
    settextcolor(RGB(255, 85, 185));
    outtextxy(10, 10, text);
}

//按钮
class Button
{
public:
    Button(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
    {
        region = rect;
        //加载对应的图片
        loadimage(&img_idle, path_img_idle);
        loadimage(&img_hovered, path_img_hovered);
        loadimage(&img_pushed, path_img_pushed);
    }

    ~Button() = default;

    void ProcessEvent(const ExMessage& msg)//对鼠标移动，左键按下 抬起消息，分别处理
    {
        switch (msg.message)
        {

        case WM_MOUSEMOVE: 
            if (status == Status::Idle && CheckCursortHit(msg.x, msg.y))
            {
                status = Status::Hovered;
            }

            else if(status == Status::Hovered && !CheckCursortHit(msg.x, msg.y))
            {
                status = Status::Idle;
            }
            break;

        case WM_LBUTTONDOWN:
            if(CheckCursortHit(msg.x, msg.y))
            {
                status = Status::Pushed;
            }
            break;

        case WM_LBUTTONUP:
            if (status == Status::Pushed)
            {
                OnClick();
            }
            break;

        default:
            break;

        }
    }


    void Draw()//根据现有状态进行绘制
    {
        switch (status)
        {
        case Status::Idle://状态触发
            putimage(region.left, region.top, &img_idle);
            break;

        case Status::Hovered:
            putimage(region.left, region.top, &img_hovered);
            break;

        case Status::Pushed:
            putimage(region.left, region.top, &img_pushed);
            break;
        }
    }

protected://定义为纯虚函数
    virtual void OnClick() = 0;
    //后续继承Button类的函数都必须实现自己的OnClick函数逻辑,才能够被实例化为对象
private:
    enum class Status//状态枚举变量
    {
        Idle = 0,
        Hovered,
        Pushed
    };

private:
    RECT region; // 用来描述自己的位置和大小
    //按钮的三张图的预载
    IMAGE img_idle;
    //正常
    IMAGE img_hovered;
    //悬停
    IMAGE img_pushed;
    //按下
    Status status = Status::Idle;

private:
    //检测鼠标点击
    bool CheckCursortHit(int x, int y)
    {//检测鼠标是否出现在鼠标按钮区域范围内
        return x >= region.left && x <= region.right && y > region.top && y <= region.bottom;
    }
};

//开始游戏按钮类
class StartGameButton : public Button
{
public:
    StartGameButton(RECT rect, LPCTSTR path_img_idle , LPCTSTR path_img_hovered , LPCTSTR path_img_pushed)
        :Button(rect , path_img_idle , path_img_hovered , path_img_pushed){}

    ~StartGameButton() = default;

protected:
    void OnClick()
    {
        is_game_started = true;//游戏开始

        mciSendString(_T("play bgm repeat from 0"), NULL, 0, NULL);
        //循环播放bgm音乐
    }
};

//退出游戏按钮类
class QuitGameButton : public Button
{
    QuitGameButton(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
        :Button(rect, path_img_idle, path_img_hovered, path_img_pushed) {}

    ~QuitGameButton() = default;

protected:
    void OnClick()
    {
        running = false;
    }
};

//主函数
int main()
{
    //初始化指针（享元模式优化)
    atlas_player_left = new Atlas(_T("img/player_left_%d.png"), 6);

    atlas_player_right = new Atlas(_T("img/player_right_%d.png"), 6);

    atlas_enemy_left = new Atlas(_T("img/enemy_left_%d.png"), 6);

    atlas_enemy_right = new Atlas(_T("img/enemy_right_%d.png"), 6);

    //按钮所在的矩形
    RECT region_btn_start_game, region_btn_quit_game;

    //开始按钮图形放置逻辑
    region_btn_start_game.left = (WINDOW_WIDTH - BUTTON_WIDTH) / 2;

    region_btn_start_game.right = region_btn_start_game.left + BUTTON_WIDTH;
    
    region_btn_start_game.top = 430;

    region_btn_start_game.bottom = region_btn_start_game.top + BUTTON_HEIGHT;

    //退出按钮图形放置逻辑
    region_btn_quit_game.left = (WINDOW_WIDTH - BUTTON_WIDTH) / 2;

    region_btn_quit_game.right = region_btn_quit_game.left + BUTTON_WIDTH;

    region_btn_quit_game.top = 550;

    region_btn_quit_game.bottom = region_btn_quit_game.top + BUTTON_HEIGHT;
    //按钮定义部分
    StartGameButton btnStart = StartGameButton(region_btn_start_game, _T("img/ui_start_idle.png"),
        _T("img/ui_start_hovered.png"), _T("img/ui_start_pushed.png"));

    StartGameButton btnQuit = StartGameButton(region_btn_quit_game, _T("img/ui_quit_idle.png"), _T("img/ui_quit_hovered.png"),
        _T("img/ui_quit_pushed.png"));

    
    IMAGE imgMenu;
    //加载菜单
    IMAGE imgBg;
    //加载背景图
    loadimage(&imgMenu, _T("img/menu.png"));
    //check Done
    loadimage(&imgBg, _T("img/background.png"));
    //check Done
    Player player;
    //check Done
    std::vector<Enemy*> enemys;
    //check Done
    std::vector<Bullet> bullets(3);
    //定义子弹数目为3
    initgraph(1280, 720);
    //check Done
    BeginBatchDraw();
    //check Done
    ExMessage msg;
    //check Done
    
    int score = 0;
    //记录玩家得分
    mciSendString(_T("open mus/bgm.mp3 alias bgm"), NULL, 0, NULL);
    //将音乐文件加载到程序里 并且在后续的程序中将这个程序取名为bgm
    mciSendString(_T("open mus/hit.wav alias hit"), NULL, 0, NULL);
    //敌人被击中的音效
    
    while (running)
    {
        DWORD startTime = GetTickCount();
        //check Done
        TryGenerateEnemy(enemys); 
        // 1.获取输入
        UpdateBullet(player, bullets);

        while (peekmessage (&msg))
        {
            if (is_game_started)
            {
                player.ProcessEvent(msg);
            }

            else
            {
                btnStart.ProcessEvent(msg);

                btnQuit.ProcessEvent(msg);
            }
        }

        //数据处理
        if (is_game_started)//check Done
        {
            player.Move();

          //  TryGenerateEnemy(enemys);

            for (int i = 0; i < enemys.size(); i++)
            {
                enemys[i]->Move(player);
            }

            for (Enemy* eny : enemys)
            {//检测敌人与玩家的碰撞
                if (eny->CheckPlayerCollision(player))//遍历敌人列表 ，如果碰撞成功
                {
                    static TCHAR text[128];

                    _stprintf_s(text, _T("最终得分： %d !"), score);

                    MessageBox(GetHWnd(), _T("失败！"), _T("游戏结束"), MB_OK);

                    running = false;//游戏结束

                    break;
                }
                //检测子弹和敌人的碰撞
                for (const Bullet& bullet : bullets)
                {
                    if (eny->CheckBulletCollision(bullet))
                    {
                        mciSendString(_T("play hit from 0"), NULL, 0, NULL);
                        //被击中后的音效播放 无reapet即无循环播放
                        eny->Hurt();//调用受击效果

                        score++;//分数++
                    }
                }//no check

            }
            //依次遍历，移除生命值为0的敌人
            for (int i = 0; i < enemys.size(); i++)
            {
                Enemy* eny = enemys[i];

                if (!eny->CheckAlive())//检测到要删除的敌人时
                {
                    std::swap(enemys[i], enemys.back());//将要删除的部位与vector末尾交换位置

                    enemys.pop_back();//再将末尾移除

                    delete eny;//将不用的指针delete掉
                    //防止内存泄漏
                }
            }
            
        }
        
        
        

        // 2.渲染

        cleardevice();

        if (is_game_started)
        {
            putimage(0, 0, &imgBg); // 将背景图片放置于窗口（ 0 ， 0 ）的位置

            player.Draw(1000 / 144);

            for (int i = 0; i < enemys.size(); i++)
            {
                enemys[i]->Draw(1000 / 144);
            }
            //绘制子弹
            for (const Bullet& bullet : bullets)
            {
                bullet.Draw();
            }

            DrawPlayerScore(score);//调用分数绘制函数
        }

        else
        {
            putimage(0, 0, &imgMenu);

            btnStart.Draw();

            btnQuit.Draw();
        }
        FlushBatchDraw();

        DWORD endTime = GetTickCount();

        DWORD deltaTime = endTime - startTime;

        if (deltaTime < 1000 / 144 )
        {
            Sleep(1000 / 144 - deltaTime);
        }
    }

    delete atlas_player_left;
    delete atlas_player_right;
    delete atlas_enemy_left;
    delete atlas_enemy_right;
    EndBatchDraw();

    return 0;
}
