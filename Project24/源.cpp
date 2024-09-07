//��Ԫģʽ�Ż���
#include <algorithm>
#include <bitset>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <graphics.h> // ���� EasyX ��ͼ��ͷ�ļ�
#include <iostream>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

bool is_game_started = false;//��ʶ��ǰ��Ϸ�Ƿ��Ѿ���ʼ

bool running = true;

const int WINDOW_WIDTH = 1280;

const int WINDOW_HEIGHT = 720;

const int BUTTON_WIDTH = 192;

const int BUTTON_HEIGHT = 75;
class Atlas//������ʹ�õ�ͼ��(��Ԫģʽ�Ż���
{
public:
    Atlas(LPCTSTR path, int num)
    {
        TCHAR path_file[256];
        for (int i = 0; i < num; i++)
        {

            _stprintf_s(path_file, path, i);

            IMAGE* frame = new IMAGE();//��Ϊʹ����new������Ҫ�ͷŵ�
            //                           һ��malloc ��Ӧһ��free ��һ��new Ӧ�ö�Ӧһ�� delete
            loadimage(frame, path_file);

            mFrameList.push_back(frame);//��ָ�����뵽������
        }
    }//check Done

    ~Atlas()
    {
        for (auto frame : mFrameList)//�ͷ��ڴ�
        {
            delete frame;
        }
    }//check Done

public:
    std::vector<IMAGE*> mFrameList;
};

//������װ��

class Animation//ʹ�÷�װ���߼�ʹ�ô�����Ӽ��

{
    int mFrameIndex = 0;//����֡����

    int mTimer = 0;//������ʱ�� ʹ�ü�ʱ��������������Ҫ���㶯�������ȶ��Ե�����

    int mIntervalMs = 0;

    std::vector<IMAGE*> mFrameList; 

public://��ʱ�ѳ�Ϊ�����ʲ��������ͷ�Atlas��ָ��
    Animation(Atlas* atlas , int interval)//�ļ����ڵ�·��  ��ǰ����ʹ�õ�ͼƬ����
    {
        anim_atlas = atlas;

        mIntervalMs = interval;
    }

    ~Animation() = default;

    //check fine

    void Play(int x, int y, int delta)//�������� ��Ⱦ������λ�� ���Լ���ȡ�˶��
    {

        mTimer += delta;

        if (mTimer >= mIntervalMs)//��ʱ������֡�������ô�����л�����ͼƬ����һ֡
        {
            mFrameIndex = (mFrameIndex + 1) % anim_atlas -> mFrameList.size();

            mTimer = 0; //�����ö�ʱ����ֵ
        }

        DrawImage(x, y, anim_atlas -> mFrameList[mFrameIndex]); //ͨ������ĺ������Ƶ�ǰ�Ķ���֡
    }
    //check fine
    //����͸��ͼ��

    // https://learn.microsoft.com/zh-cn/windows/win32/api/wingdi/nf-wingdi-alphablend?redirectedfrom=MSDN

    static void DrawImage(int x, int y, IMAGE* img) //����͸�����ĺ���,��ȥ���زĺڿ�
    {

        int w = img->getwidth();

        int h = img->getheight();

        AlphaBlend(GetImageHDC(NULL), x, y, w, h,

            GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA });
    }
private:
    Atlas* anim_atlas;
};
//���õĶ��� ��ָ�붨��Ϊȫ�ֱ���

Atlas* atlas_player_left;
Atlas* atlas_player_right;
Atlas* atlas_enemy_left;
Atlas* atlas_enemy_right;
#pragma once
//����ʵ��
//Animation anim_left_player(_T("img/player_left_%d.png"), 6, 45);
//Animation anim_right_player(_T("img/player_right_%d.png"), 6, 45);
//�����

class Player//�����
{

    bool mIsMoveLeft = false;

    bool mIsMoveRight = false;

    bool mIsMoveUp = false;

    bool mIsMoveDown = false;

    int PLAYER_SPEED = 3; //��Ҷ�����֡��

    POINT mPos = { 720, 360 }; //���λ��

    //��Drawʵ����Ӱλ�õĶ�λ
    IMAGE mImgShadow; //��Ӱͼ��

    Animation* mAnimLeft; //������֡����

    Animation* mAnimRight; //���Ҷ���֡����

public:

    const int FRAME_WIDTH = 80; //���ͼ����

    const int FRAME_HEIGHT = 80; //���ͼ��߶�

    const int SHADOW_WIDTH = 32; //�����Ӱͼ����

    Player()

    {

        loadimage(&mImgShadow, _T("img/shadow_player.png"));

        mAnimLeft = new Animation(atlas_player_left , 45); //����һ����6��ͼƬ���ɣ�֡���45����

        mAnimRight = new Animation(atlas_player_right, 45);
    }

    ~Player()

    {

        delete mAnimLeft;

        delete mAnimRight;
    }

    void ProcessEvent(const ExMessage& msg)

    {

        if (msg.message == WM_KEYDOWN)//ʵ�������ƶ�����
        {                              //����������ʱ

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
        {//������̧��ʱ

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

    void Move()//�ƶ��߼�
    {

        int dirX = mIsMoveRight - mIsMoveLeft;//x�����

        int dirY = mIsMoveDown - mIsMoveUp;//y�����

        double dirLen = sqrt(dirX * dirX + dirY * dirY);//ֱ�߾���

        if (dirLen != 0)
        {

            double normalizedX = dirX / dirLen;

            double normalizedY = dirY / dirLen;

            mPos.x += (int)(PLAYER_SPEED * normalizedX);

            mPos.y += (int)(PLAYER_SPEED * normalizedY);
        }
        //�߽����ƣ�ʹ����ҽ����ڴ������˶�
        mPos.x = mPos.x < 0 ? 0 : mPos.x;

        mPos.x = mPos.x > 1200 ? 1200 : mPos.x;

        mPos.y = mPos.y < 0 ? 0 : mPos.y;

        mPos.y = mPos.y > 640 ? 640 : mPos.y;
    } // x

    void Draw(int delta)

    {

        int shadowPosX = mPos.x + (FRAME_WIDTH / 2 - SHADOW_WIDTH / 2);//ʹ����ӰͼƬλ�þ���

        int shadowPosY = mPos.y + FRAME_HEIGHT - 8;//��Ӱ������y���λ��

        Animation::DrawImage(shadowPosX, shadowPosY, &mImgShadow);
        //ʵ�������ƶ�ʱ������ת
        static bool facingLeft = false;//����һ��˽�б�����ȷ���Ƿ�������

        int dirX = mIsMoveRight - mIsMoveLeft;

        if (dirX < 0)//���x���ϵ��ƶ�����С��0

            facingLeft = true;

        else if (dirX > 0)

            facingLeft = false;

        if (facingLeft)//������ֵ�ж�����������ҵĶ���

            mAnimLeft->Play(mPos.x, mPos.y, delta);

        else

            mAnimRight->Play(mPos.x, mPos.y, delta);
    }

    const POINT& GetPos() const//����������ҵ�ǰλ��

    {

        return mPos;
    }
};

#pragma once
//�ӵ���

class Bullet//�ӵ���
{
public:
    POINT mPos = { 0 , 0 };
public:
    Bullet() = default;
    ~Bullet() = default;

    void Draw() const//��ɫ��״
    {
        setlinecolor(RGB(255, 155, 50));
        setfillcolor(RGB(255, 155, 50));
        fillcircle(mPos.x, mPos.y, RADIUS);
    }
private:
    const int RADIUS = 10;
};
//������

class Enemy//���� ��

{

    const int SPEED = 2; //�ƶ��ٶ�

    const int FRAME_WIDTH = 80; //֡��

    const int FRAME_HEIGHT = 80; //֡��

    const int SHADOW_WIDTH = 48; //��Ӱ��

    IMAGE mImgShadow; //��Ӱͼ��

    Animation* mAnimLeft; //������֡����

    Animation* mAnimRight; //���Ҷ���֡����

    POINT mPos = { 0, 0 }; //λ��

    bool mFacingLeft = false; //�����˶���־λ

    bool mIsAlive = true;//�Ƿ����

    //no check
public:
    Enemy()//���˲����߼�
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

        SpawnEdge edge = (SpawnEdge)(rand() % 4);//ʹ���������
        //�����������ҵ���������߼�
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

    bool CheckBulletCollision(const Bullet& bullet)//�ӵ���ײ���
    {                         //��������ã�������ʹ��ָ�봫�����������˶Դ����������˲���Ҫ�Ŀ�������//ʹ��const����
                              //�ں����ڲ���С�ĶԲ����������޸�
        bool BetweenX = (bullet.mPos.x > mPos.x && bullet.mPos.x < (mPos.x + FRAME_WIDTH)) ? true : false;
        //�жϱ߽磬������������
        bool BetweenY = (bullet.mPos.y > mPos.y && bullet.mPos.y < (mPos.y + FRAME_HEIGHT)) ? true : false;

        return BetweenX && BetweenY;
    }

    bool CheckPlayerCollision(const Player& player)//�����ײ���
    {
        //�����˵�����λ�õ�ЧΪ�㣬�жϵ��Ƿ�����Ҿ�����ײ����
        bool BetweenX = ((mPos.x + FRAME_WIDTH / 2) > player.GetPos().x
            && (mPos.x + FRAME_WIDTH / 2) < (player.GetPos().x + FRAME_WIDTH)) ? true : false;

        bool BetweenY = ((mPos.y + FRAME_HEIGHT / 2) > player.GetPos().y
            && (mPos.y + FRAME_HEIGHT / 2) < (player.GetPos().y + FRAME_HEIGHT)) ? true : false;

        return BetweenX && BetweenY;
    }

    void Move(const Player& player)//ʼ��׷����ҵ��ƶ�
    {
        const POINT& playerPos = player.GetPos();

        int dirX = playerPos.x - mPos.x;//�����x�����

        int dirY = playerPos.y - mPos.y;//Y��

        double dirLen = sqrt(dirX * dirX + dirY * dirY); //����Ҿ���

        if (dirLen != 0)
        {

            double normalizedX = dirX / dirLen; // X�᷽��

            double normalizedY = dirY / dirLen; // Y�᷽��

            mPos.x += (int)(SPEED * normalizedX);

            mPos.y += (int)(SPEED * normalizedY);
        }

        if (dirX < 0)//�ж�
        {
            mFacingLeft = true;
        }
        else if (dirX > 0)
        {
            mFacingLeft = false;
        }
    }

    void Draw(int delta)//����Ԥ��
    {

        int shadowPosX = mPos.x + (FRAME_WIDTH / 2 - SHADOW_WIDTH / 2);

        int shadowPosY = mPos.y + FRAME_HEIGHT - 35;

        Animation::DrawImage(shadowPosX, shadowPosY, &mImgShadow);

        if (mFacingLeft)

            mAnimLeft->Play(mPos.x, mPos.y, delta);

        else

            mAnimRight->Play(mPos.x, mPos.y, delta);
    }

    const POINT& GetPos() const//ȡ��λ��
    {

        return mPos;
    }
    //�������Ƿ����
    void Hurt()//�ܻ�����
    {
        mIsAlive = false;//���ӵ����м���
    }

    bool CheckAlive()
    {
        return mIsAlive;
    }

};

#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "Msimg32.lib")

//�����µĵ���
void TryGenerateEnemy(std::vector<Enemy*>& enemyList)//��������
{
    const int INTERVAL = 100;
    //���ü��������ڹ̶�ʱ������������
    static int counter = 0;

    if ((++counter) % INTERVAL == 0)
    {
        enemyList.push_back(new Enemy());
    }   
}

//�ӵ������߼�ʵ��
void UpdateBullet(const Player& player, std::vector<Bullet>& bullets)//������סѭ����ʵ���ӵ�ʵʱ�������ǵ��߼�
{//���ӵ�ӵ����Բ���˶��������ٶ��⣬����һ�����ϲ����ľ����ٶ�
    const double RADIAL_SPEED = 0.0045; // ���򲨶��ٶ� �������ӵ��������ʱԶʱ���Ĳ����ٶ�

    const double TANGENT_SPEED = 0.0055;// ���򲨶��ٶ� ������Բ���ٶȵĿ���

    double radian_interval = 2 * 3.14159 / bullets.size(); // �ӵ�֮��Ļ��ȼ�� ʹ��360�ȳ����ӵ��ĸ���
    //�����ӵ��б��е�ÿһ���ӵ�
    double radius = 100 + 25 * sin(GetTickCount() * RADIAL_SPEED);//�ӵ���ת��ʵ�� ����ʱ�����ӵ���GetTickCount * speed���Ƕȼ���

    for (size_t i = 0; i < bullets.size(); i++)
    {
        double radian = GetTickCount() * TANGENT_SPEED + radian_interval * i;// ��ǰ�ӵ����ڻ���ֵ

        bullets[i].mPos.x = player.GetPos().x + player.FRAME_WIDTH / 2 + (int)(radius * sin(radian));

        bullets[i].mPos.y = player.GetPos().y + player.FRAME_HEIGHT / 2 + (int)(radius * cos(radian));
    }

}

//���Ʒ���
void DrawPlayerScore(int score)
{
    static TCHAR text[64];
    _stprintf_s(text, _T("��ǰ��ҵ÷֣� %d"), score);

    setbkmode(TRANSPARENT);
    settextcolor(RGB(255, 85, 185));
    outtextxy(10, 10, text);
}

//��ť
class Button
{
public:
    Button(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
    {
        region = rect;
        //���ض�Ӧ��ͼƬ
        loadimage(&img_idle, path_img_idle);
        loadimage(&img_hovered, path_img_hovered);
        loadimage(&img_pushed, path_img_pushed);
    }

    ~Button() = default;

    void ProcessEvent(const ExMessage& msg)//������ƶ���������� ̧����Ϣ���ֱ���
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


    void Draw()//��������״̬���л���
    {
        switch (status)
        {
        case Status::Idle://״̬����
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

protected://����Ϊ���麯��
    virtual void OnClick() = 0;
    //�����̳�Button��ĺ���������ʵ���Լ���OnClick�����߼�,���ܹ���ʵ����Ϊ����
private:
    enum class Status//״̬ö�ٱ���
    {
        Idle = 0,
        Hovered,
        Pushed
    };

private:
    RECT region; // ���������Լ���λ�úʹ�С
    //��ť������ͼ��Ԥ��
    IMAGE img_idle;
    //����
    IMAGE img_hovered;
    //��ͣ
    IMAGE img_pushed;
    //����
    Status status = Status::Idle;

private:
    //��������
    bool CheckCursortHit(int x, int y)
    {//�������Ƿ��������갴ť����Χ��
        return x >= region.left && x <= region.right && y > region.top && y <= region.bottom;
    }
};

//��ʼ��Ϸ��ť��
class StartGameButton : public Button
{
public:
    StartGameButton(RECT rect, LPCTSTR path_img_idle , LPCTSTR path_img_hovered , LPCTSTR path_img_pushed)
        :Button(rect , path_img_idle , path_img_hovered , path_img_pushed){}

    ~StartGameButton() = default;

protected:
    void OnClick()
    {
        is_game_started = true;//��Ϸ��ʼ

        mciSendString(_T("play bgm repeat from 0"), NULL, 0, NULL);
        //ѭ������bgm����
    }
};

//�˳���Ϸ��ť��
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

//������
int main()
{
    //��ʼ��ָ�루��Ԫģʽ�Ż�)
    atlas_player_left = new Atlas(_T("img/player_left_%d.png"), 6);

    atlas_player_right = new Atlas(_T("img/player_right_%d.png"), 6);

    atlas_enemy_left = new Atlas(_T("img/enemy_left_%d.png"), 6);

    atlas_enemy_right = new Atlas(_T("img/enemy_right_%d.png"), 6);

    //��ť���ڵľ���
    RECT region_btn_start_game, region_btn_quit_game;

    //��ʼ��ťͼ�η����߼�
    region_btn_start_game.left = (WINDOW_WIDTH - BUTTON_WIDTH) / 2;

    region_btn_start_game.right = region_btn_start_game.left + BUTTON_WIDTH;
    
    region_btn_start_game.top = 430;

    region_btn_start_game.bottom = region_btn_start_game.top + BUTTON_HEIGHT;

    //�˳���ťͼ�η����߼�
    region_btn_quit_game.left = (WINDOW_WIDTH - BUTTON_WIDTH) / 2;

    region_btn_quit_game.right = region_btn_quit_game.left + BUTTON_WIDTH;

    region_btn_quit_game.top = 550;

    region_btn_quit_game.bottom = region_btn_quit_game.top + BUTTON_HEIGHT;
    //��ť���岿��
    StartGameButton btnStart = StartGameButton(region_btn_start_game, _T("img/ui_start_idle.png"),
        _T("img/ui_start_hovered.png"), _T("img/ui_start_pushed.png"));

    StartGameButton btnQuit = StartGameButton(region_btn_quit_game, _T("img/ui_quit_idle.png"), _T("img/ui_quit_hovered.png"),
        _T("img/ui_quit_pushed.png"));

    
    IMAGE imgMenu;
    //���ز˵�
    IMAGE imgBg;
    //���ر���ͼ
    loadimage(&imgMenu, _T("img/menu.png"));
    //check Done
    loadimage(&imgBg, _T("img/background.png"));
    //check Done
    Player player;
    //check Done
    std::vector<Enemy*> enemys;
    //check Done
    std::vector<Bullet> bullets(3);
    //�����ӵ���ĿΪ3
    initgraph(1280, 720);
    //check Done
    BeginBatchDraw();
    //check Done
    ExMessage msg;
    //check Done
    
    int score = 0;
    //��¼��ҵ÷�
    mciSendString(_T("open mus/bgm.mp3 alias bgm"), NULL, 0, NULL);
    //�������ļ����ص������� �����ں����ĳ����н��������ȡ��Ϊbgm
    mciSendString(_T("open mus/hit.wav alias hit"), NULL, 0, NULL);
    //���˱����е���Ч
    
    while (running)
    {
        DWORD startTime = GetTickCount();
        //check Done
        TryGenerateEnemy(enemys); 
        // 1.��ȡ����
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

        //���ݴ���
        if (is_game_started)//check Done
        {
            player.Move();

          //  TryGenerateEnemy(enemys);

            for (int i = 0; i < enemys.size(); i++)
            {
                enemys[i]->Move(player);
            }

            for (Enemy* eny : enemys)
            {//����������ҵ���ײ
                if (eny->CheckPlayerCollision(player))//���������б� �������ײ�ɹ�
                {
                    static TCHAR text[128];

                    _stprintf_s(text, _T("���յ÷֣� %d !"), score);

                    MessageBox(GetHWnd(), _T("ʧ�ܣ�"), _T("��Ϸ����"), MB_OK);

                    running = false;//��Ϸ����

                    break;
                }
                //����ӵ��͵��˵���ײ
                for (const Bullet& bullet : bullets)
                {
                    if (eny->CheckBulletCollision(bullet))
                    {
                        mciSendString(_T("play hit from 0"), NULL, 0, NULL);
                        //�����к����Ч���� ��reapet����ѭ������
                        eny->Hurt();//�����ܻ�Ч��

                        score++;//����++
                    }
                }//no check

            }
            //���α������Ƴ�����ֵΪ0�ĵ���
            for (int i = 0; i < enemys.size(); i++)
            {
                Enemy* eny = enemys[i];

                if (!eny->CheckAlive())//��⵽Ҫɾ���ĵ���ʱ
                {
                    std::swap(enemys[i], enemys.back());//��Ҫɾ���Ĳ�λ��vectorĩβ����λ��

                    enemys.pop_back();//�ٽ�ĩβ�Ƴ�

                    delete eny;//�����õ�ָ��delete��
                    //��ֹ�ڴ�й©
                }
            }
            
        }
        
        
        

        // 2.��Ⱦ

        cleardevice();

        if (is_game_started)
        {
            putimage(0, 0, &imgBg); // ������ͼƬ�����ڴ��ڣ� 0 �� 0 ����λ��

            player.Draw(1000 / 144);

            for (int i = 0; i < enemys.size(); i++)
            {
                enemys[i]->Draw(1000 / 144);
            }
            //�����ӵ�
            for (const Bullet& bullet : bullets)
            {
                bullet.Draw();
            }

            DrawPlayerScore(score);//���÷������ƺ���
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
