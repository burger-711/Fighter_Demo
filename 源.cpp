#include <iostream>
#include <graphics.h>//easyX
#include <vector>//����
#include <conio.h>
#include <Windows.h>
using namespace std;

constexpr auto swidth = 600;
constexpr auto sheight = 1100;

constexpr unsigned int hp = 3;
constexpr auto deadtime = 1000;//ms
bool PointInRect(int x,int y,RECT& r)//����Ƿ������ο�
{
	return(r.left <= x && x <= r.right && r.top <= y && y <= r.bottom);
}

bool RectCrash(RECT &r1, RECT &r2)//�ж��Ƿ���ײ
{
	RECT r;
	r.left = r1.left - (r2.right - r2.left);
	r.right = r1.right;
	r.top = r1.top - (r2.bottom - r2.top);
	r.bottom = r1.bottom;

	return(r.left < r2.left && r2.left <= r.right && r.top <= r2.top && r2.top <= r.bottom);
}
//��Ϸ��ʼ����
void welcome()
{
	LPCTSTR title = _T("�ɻ���ս");
	LPCTSTR tplay = _T("��ʼ��Ϸ");
	LPCTSTR texit = _T("�˳���Ϸ");            

	RECT tplayr, texitr;//RECT���������洢һ�����ο�����Ͻ����ꡢ��Ⱥ͸߶�
	BeginBatchDraw();
	setbkcolor(WHITE);
	cleardevice();
	settextstyle(60, 0, _T("����"));
	settextcolor(BLACK);
	outtextxy(swidth / 2 - textwidth(title) / 2, sheight / 8, title);
    
	settextstyle(40, 0, _T("����"));
	tplayr.left = swidth / 2 - textwidth(tplay) / 2;
	tplayr.right = tplayr.left + textwidth(tplay);
	tplayr.top = sheight / 5*1.5;
	tplayr.bottom = tplayr.top + textheight(tplay);

	texitr.left = swidth / 2 - textwidth(texit) / 2;
	texitr.right = texitr.left + textwidth(texit);
	texitr.top = sheight / 5*2;
	texitr.bottom = texitr.top + textheight(texit);
	
	outtextxy(tplayr.left, tplayr.top, tplay);
	outtextxy(texitr.left, texitr.top, texit);

	EndBatchDraw();
	while (true)
	{
		ExMessage mess;
		getmessage(&mess, EX_MOUSE);//��ȡ�����Ϣ
		if (mess.lbutton)//�ж��Ƿ�����һ���Ϣ
		{
			if(PointInRect(mess.x,mess.y,tplayr))//�ж��Ƿ��ڡ���ʼ��Ϸ�����ο���
			{
				return;
			}
			else if (PointInRect(mess.x, mess.y, texitr))
			{
				exit(0);
			}
		}

	}
}

void gameover(unsigned long long &kill)
{
	TCHAR* str = new TCHAR[128];
	_stprintf_s(str, 128, _T("��ɱ����%llu"), kill);
	settextcolor(RED);
	outtextxy(swidth / 2 - textwidth(str) / 2, sheight / 5, str);
	
	//�����¼� ��Enter����
	LPCTSTR info = _T("��Enter����");
	settextstyle(20, 0, _T("����"));
	outtextxy(swidth - textwidth(info), sheight - textheight(info), info);

	while (true)
	{
		ExMessage mess;
		getmessage(&mess, EM_KEY);
		if (mess.vkcode == 0x0D)
		{
			return;
		}

	}

}

//�������л���Ӣ�ۡ��ӵ�

class BK
{
public:
	BK(IMAGE& img)
		:img(img), y(-sheight){}
	void show()
	{
		if (y == 0) { y = -sheight; }
		y += 4;
		putimage(0, y, &img);
	}
private:
	IMAGE& img;
	int y;
};

class Hero
{
public:
	Hero(IMAGE& img)
		:img(img),HP(hp)
	{
		rect.left = swidth / 2 - img.getwidth() / 2;
		rect.top = sheight - img.getheight();
		rect.right = rect.left + img.getwidth();
		rect.bottom = sheight;
	}
	void show()
	{
		setlinecolor(RED);
		setlinestyle(PS_SOLID, 4);
		putimage(rect.left, rect.top, &img);
		line(rect.left, rect.top - 5, rect.left + (img.getwidth() / hp * HP), rect.top - 5);
	}
	void control()
	{
		ExMessage mess;
		if (peekmessage(&mess, EM_MOUSE))
		{
			rect.left = mess.x- img.getwidth() / 2;//����ƶ���������
			rect.top = mess.y- img.getheight() / 2;
			rect.right  = rect.left + img.getwidth();
			rect.bottom = rect.top + img.getheight();
		}
	}
	bool dead()
	{
		HP--;
		return(HP == 0) ? false: true;
	}
	RECT& GetRect() { return rect; }
private:
	IMAGE& img;
	RECT rect;

	unsigned int HP;
};

class Enemy
{
public:
	Enemy(IMAGE& img, int x,IMAGE*& boom)
		:img(img),isdie(false),boomsum(0)
	{
		selfboom[0] = boom[0];
		selfboom[1] = boom[1];
		selfboom[2] = boom[2];
		rect.left = x;
		rect.right = rect.left + img.getwidth();
		rect.top = -img.getheight();
		rect.bottom = 0;
	}
	bool show()
	{
		if (isdie)
		{
			if (boomsum == 3)
			{
				return false;
			}
			putimage(rect.left, rect.top,selfboom+boomsum);
			boomsum++;

			return true;
		}
		if (rect.top >= sheight)
		{
			return false;
		}
		rect.top += 4;
		rect.bottom += 4;
		putimage(rect.left, rect.top, &img);

		return true;
	}
	void Isdie()
	{
		isdie = true;
	}
	RECT& GetRect() { return rect; }
private:
	IMAGE& img;
	RECT rect;
	IMAGE selfboom[3];

	bool isdie;
	int boomsum;
};

class Bullet
{
public:
	Bullet(IMAGE& img, RECT pr)
		:img(img)
	{
		rect.left = pr.left + (pr.right - pr.left) / 2 - img.getwidth() / 2;
		rect.right = rect.left + img.getwidth();
		rect.top = pr.top - img.getheight();
		rect.bottom = rect.top + img.getheight();
	}
	bool show()
	{
		if (rect.bottom <= 0)
		{
			return false;
		}
		rect.top -= 3;
		rect.bottom -= 3;
		putimage(rect.left, rect.top, &img);

		return true;
	}
	RECT& GetRect() { return rect; }
protected:
	IMAGE& img;
	RECT rect;
};

class EBullet : public Bullet
{
public:
	EBullet(IMAGE& img, RECT pr)
		:Bullet(img, pr)
	{
		rect.left = pr.left + (pr.right - pr.left) / 2 - img.getwidth() / 2;
		rect.right = rect.left + img.getwidth();
		rect.top = pr.bottom;
		rect.bottom = rect.top + img.getheight();
	}
	bool show()
	{
		if (rect.top >= sheight)
		{
			return false;
		}
		rect.top += 5;
		rect.bottom += 5;
		putimage(rect.left, rect.top, &img);

		return true;
	}
};


bool AddEnemy(vector<Enemy*>& enemies,IMAGE& enemyimg,IMAGE* boom)
{
	Enemy* e = new Enemy(enemyimg, abs(rand()) % (swidth - enemyimg.getwidth()),boom);
	for (auto& i : enemies)
	{
		if (RectCrash(i->GetRect(), e->GetRect()))//�жϵл������Ƿ���ײ
		{
			delete e;
			return false;
		}
	}
	enemies.push_back(e);//push_back()������һ���µ�Ԫ�ؼӵ�vector������棬λ��Ϊ��ǰ���һ��Ԫ�ص���һ��Ԫ��
	return true;
}



bool Play()
{
	setbkcolor(WHITE);
	cleardevice();
	bool is_play = true;

	IMAGE heroimg, enemyimg, bkimg, bulletimg,ebulletimg;
	IMAGE eboom[3];

	loadimage(&heroimg, _T("D:\\projects\\PlaneFight\\images\\me1.png"));
	loadimage(&enemyimg, _T("D:\\projects\\PlaneFight\\images\\enemy1.png"));
	loadimage(&bkimg, _T("D:\\projects\\PlaneFight\\images\\bk2.png"),swidth,sheight*2);
	loadimage(&bulletimg, _T("D:\\projects\\PlaneFight\\images\\bullet1.png"));
	loadimage(&ebulletimg, _T("D:\\projects\\PlaneFight\\images\\bullet2.png"));

	loadimage(&eboom[0], _T("D:\\projects\\PlaneFight\\images\\enemy1_down1.png"));
	loadimage(&eboom[1], _T( "D:\\projects\\PlaneFight\\images\\enemy1_down2.png"));
	loadimage(&eboom[2], _T("D:\\projects\\PlaneFight\\images\\enemy1_down3 .png"));

	BK bk = BK(bkimg);
	Hero hp = Hero(heroimg);

	vector<Enemy*> enemies;//�л���������
	vector<Bullet*> bs;//�ӵ�����
	vector<EBullet*> ebs;
	int fps = 0;//֡����ʱ

	clock_t deadlast = clock();

	unsigned long long kill = 0;
	for (int i = 0; i < 5; i++)
	{
		AddEnemy(enemies, enemyimg,eboom);
	}
	bool isPaused = false; // ��Ϸ״̬����ʼΪ����״̬
	while (is_play)
	{
	
		if (!isPaused) 
		{
		fps++;
		if (fps % 20 == 0)//��֡������һ���ӵ�
		{
			bs.push_back(new Bullet(bulletimg, hp.GetRect()));
		}
		if (fps == 60)
		{
			fps = 0;
			for (auto& i : enemies)
			{
				ebs.push_back(new EBullet(ebulletimg, i->GetRect()));
			}
		}

		BeginBatchDraw();

		bk.show();
		Sleep(2);
		flushmessage();//ˢ����Ϣ������
		Sleep(2);
		hp.control();
		
		
		hp.show();

		auto bsit = bs.begin();
		while (bsit != bs.end())
		{
			if (!(*bsit)->show())
			{
				bsit = bs.erase(bsit);
			}
			else
			{
				bsit++;
			}
		}
		auto ebsit = ebs.begin();
		while (ebsit != ebs.end())
		{
			if (!(*ebsit)->show())
			{
				ebsit = ebs.erase(ebsit);
			}

			else
			{
				if (RectCrash((*ebsit)->GetRect(), hp.GetRect()))
				{
					if (clock() - deadlast >= deadtime)
					{
						is_play = hp.dead();
						deadlast = clock();
					}
				}
				ebsit++;
			}

		}

		auto it = enemies.begin();
		while (it != enemies.end())//����ÿ���л�
		{
			if (RectCrash((*it)->GetRect(), hp.GetRect()))
			{
				if (clock() - deadlast >= deadtime)
				{
					is_play = hp.dead();
					deadlast = clock();
				}
			}
			auto bit = bs.begin();
			while (bit != bs.end())//ÿ���л�����ÿ���ӵ�
			{
				if (RectCrash((*bit)->GetRect(), (*it)->GetRect()))//�ӵ��Ƿ���ел�
				{
					(*it)->Isdie();
					delete (*bit);
					bs.erase(bit);

					kill++;
					break;//���ел����л����ӵ����٣��˳��ӵ�����
				}
				bit++;
			}
			if (!(*it)->show())
			{
				delete (*it);//ɾ����ָ�룬��Ϊ��ָ����new������
				enemies.erase(it);//��������ɾ��
				it = enemies.begin();
			}
			it++;
		}
		for (int i = 0; i < 5 - enemies.size(); i++)//�������ɵл�
		{
			AddEnemy(enemies, enemyimg, eboom);
		}
		EndBatchDraw();
	}
		// ���������¼�
		while (GetAsyncKeyState(VK_SPACE) & 0x8000) // �û�������ͣ��
		{
			if (!isPaused)
			{
				Sleep(10);
				isPaused = true; // ������Ϸ״̬Ϊ��ͣ״̬
				flushmessage();
				break;
			}
			else if (isPaused) // �û����¼�����
			{
				isPaused = false; // ������Ϸ״̬Ϊ����״̬
				flushmessage();
				break;
			}
		}
		// �ȴ�һ��ʱ��
		Sleep(10); // ������Ϸ֡��

	}
	gameover(kill);

	return true;
}

int main()
{
	//easyX��ʼ��
	initgraph(swidth, sheight, EX_NOMINIMIZE | EX_SHOWCONSOLE);
	bool is_live = true;
	while(is_live)
	{
		welcome();
		//Play
		is_live = Play();
	}
	return 0;
}