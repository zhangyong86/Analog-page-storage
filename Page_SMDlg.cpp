
// Page_SMDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Page_SM.h"
#include "Page_SMDlg.h"
#include "afxdialogex.h"
#include<iostream>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
CPage_SMDlg * pp;
CWinThread* mythreadURL;
CString *psc;//数组指针
int *psi;
int m_size;//驻留个数
int M_time; //内存读取时间
int Q_time;//缺页中断时间
int K_time;//快表时间
struct mem//用于LRU
{
	int num;
	int count;//未使用的次数
}memBlock[3] = { 0, 0, 0, 0, 0, 0 };//结构体数组初始化，内存页面数为3
int memBlockF[3] = { 0, 0, 0 };//用于FIFO

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPage_SMDlg 对话框



CPage_SMDlg::CPage_SMDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPage_SMDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPage_SMDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_Control(pDX, IDC_LIST3, m_list3);
	DDX_Control(pDX, IDC_LIST1, m_list1);
	DDX_Control(pDX, IDC_LIST2, m_list2);
	DDX_Control(pDX, IDC_LIST3, m_list3);
	DDX_Control(pDX, IDC_LIST5, m_list5);
	DDX_Control(pDX, IDC_LIST7, m_ctllist);
}

BEGIN_MESSAGE_MAP(CPage_SMDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CPage_SMDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDOK, &CPage_SMDlg::OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT1, &CPage_SMDlg::OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_BUTTON2, &CPage_SMDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON4, &CPage_SMDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON3, &CPage_SMDlg::OnBnClickedButton3)
	ON_EN_KILLFOCUS(IDC_EDIT2, &CPage_SMDlg::OnEnKillfocusEdit2)
	ON_EN_KILLFOCUS(IDC_EDIT3, &CPage_SMDlg::OnEnKillfocusEdit3)
	ON_EN_KILLFOCUS(IDC_EDIT4, &CPage_SMDlg::OnEnKillfocusEdit4)
	ON_EN_KILLFOCUS(IDC_EDIT5, &CPage_SMDlg::OnEnKillfocusEdit5)
END_MESSAGE_MAP()


// CPage_SMDlg 消息处理程序

BOOL CPage_SMDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	pp = this;
	// TODO:  在此添加额外的初始化代码
	DWORD dwStyle = m_ctllist.GetExtendedStyle();//添加列表框的网格线
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;
	m_ctllist.SetExtendedStyle(dwStyle);
	m_ctllist.InsertColumn(0, "", LVCFMT_LEFT, 40);
	m_ctllist.InsertColumn(1, "总时间", LVCFMT_LEFT, 80);  //添加列标题，LVCFMT_LEFT用来设置对齐方式
	m_ctllist.InsertColumn(2, "平均时间", LVCFMT_LEFT, 80);
	m_ctllist.InsertColumn(3, "缺页中断数目", LVCFMT_LEFT, 80);
	m_ctllist.InsertItem(0, "FIFO");
	m_ctllist.InsertItem(1, "LRU");
	m_ctllist.InsertItem(2, "LFU");
	m_ctllist.InsertItem(3, "OPT");
	//m_list3.AddString((CString)"要输出的数据");
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CPage_SMDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPage_SMDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CPage_SMDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
void CPage_SMDlg::Random()
{
	/*CString a;
	CString b;
	b.Format(_T("%.1f"), (rand()+10) % 10);*/
	CString string;
	CString string2;
	/*CString string3;*/
	int b[10];
	psc = new CString[10];
	psi = new int[10];
	for (int i = 0; i < 10; i++)
	{
		b[i] = rand() % 10;
		psi[i] = b[i];
		string.Format(_T("%d"), b[i]);
		psc[i] =string;
		/*string.Format(_T("%d"), ps[i]);*/
		string2 =string2+ string;
		/*string3 = string3 + string;*/
	}
	GetDlgItem(IDC_EDIT1)->SetWindowText(string2);
	/*GetDlgItem(IDC_EDIT2)->SetWindowTextW(string3);*/

	/*for (int i = 0; i < 10; i++)
	{
		CString b;
		b.Format(_T("%.1f"),rand()%10);
		a = a +b ;
		GetDlgItem(IDC_EDIT1)->SetWindowTextW(a);
	}*/
	//return 0;
}
void CPage_SMDlg::OnBnClickedButton1()//产生随机数
{
	Random();
	// TODO:  在此添加控件通知处理程序代码
}

UINT CPage_SMDlg::threadFIFO(LPVOID lpParam)
{
	int good;
	CPage_SMDlg *ct = (CPage_SMDlg*)lpParam;
	ct->m_list1.ResetContent();
	CProgressCtrl *pProgressCtrl = (CProgressCtrl*)pp->GetDlgItem(IDC_PROGRESS1);
	ct->m_list2.ResetContent();
	//ct->m_list1.AddString(_T("要输出的数据"));
	//ct->m_list1.AddString((CString)("要输出的数据"));
	//ct->SetDlgItemTextW(IDC_STATIC, _T("要输出的数据"));statictext输出
	int jindu = 0;
	CString string;
	CString string2;
	CString string3;
	for (int i = 0; i < 10; i++)
	{
		bool bFind = false;
		for (int j = 0; j < 3; j++)
		{
			if (psi[i] == memBlockF[j])
			{
				bFind = true;
			}
		}

		if (!bFind)
		{
			for (int j = 1; i < 3; i++)
			{
				memBlockF[j] = memBlockF[j - 1];
			}
			memBlockF[0] = psi[i];
			
		}
		/*string2 = "";*/
		for (int j = 0; j < 3; j++)//输出
		{

			string.Format(_T("%d"), memBlockF[j]);
			string2 = string2 + string;

		}
		string3 = string2.Right(3);
		/*ct->m_list2.AddString('(' + 'a' + ')' + cstring2);*/
		ct->m_list1.InsertString(i,string3);
		jindu = jindu + 10;
		pProgressCtrl->SetPos(jindu);
	}
	//FIFO算法：
	return 0;
}
UINT CPage_SMDlg::threadLRU(LPVOID lpParam)
{
	int good=0;
	CPage_SMDlg *ct = (CPage_SMDlg*)lpParam;
	ct->m_list2.ResetContent();
	CProgressCtrl *pProgressCtrl = (CProgressCtrl*)pp->GetDlgItem(IDC_PROGRESS2);//控制进度
	int jindu=0;
	CString cstring;
	CString cstring2;
	CString cstring3;
	for (int i = 0; i < 10; i++)
	{
		int maxCount = memBlock[0].count;
		int maxPos = 0;
		bool bFind = false;
		for (int j = 0; j < 3; j++)
		{
			// 标记出count值最大的位置
			if (maxCount < memBlock[j].count)
			{
				maxCount = memBlock[j].count;
				maxPos = j;
			}

			// 将所有的count值都+1
			memBlock[j].count++;

			// 如果命中，将其count值置为0
			if (psi[i] == memBlock[j].num)
			{
				memBlock[j].count = 0;
				bFind = true;
				good++;
			}
		}

		// 未命中，将count最大的拿来替换
		if (!bFind)
		{
			memBlock[maxPos].num = psi[i];
			memBlock[maxPos].count = 0;
			Sleep(Q_time);
		}
		for (int j = 0; j < 3; j++)//输出
		{
			
			cstring.Format(_T("%d"), memBlock[j].num);
			/*ct->m_list2.AddString(cstring);*/
			cstring2 = cstring2 + cstring;
			
		}
		/*ct->m_list2.AddString('(' + 'a' + ')' + cstring2);*/


		cstring3= cstring2.Right(3);//取子串右边3
		ct->m_list2.InsertString(i,cstring3);
		jindu = jindu + 10;
		pProgressCtrl->SetPos(jindu);
		/*cstring2.Empty();*/
	}

	int z_time;//总时间
	int p_time;//平均时间
	int q_num;//缺页中断个数
	CString cz_time;
	z_time = (K_time + M_time)*good + (10 - good)*(K_time + Q_time);
	p_time = z_time / 10;
	cz_time.Format(_T("%d"), z_time);
	ct->m_ctllist.SetItemText(0, 1, cz_time);
	cz_time.Format(_T("%d"), p_time);
	ct->m_ctllist.SetItemText(0, 2, cz_time);
	q_num = 10 - good;
	cz_time.Format(_T("%d"), q_num);
	ct->m_ctllist.SetItemText(0, 3, cz_time);
	return 0;
}

void CPage_SMDlg::OnBnClickedOk()//开始执行
{

	for (int i = 0; i < 10; i++)
	{
		m_list5.InsertString(i,'('+ psc[i]+')');
	}
	CString str;
	//FIFO
	CWinThread* mythreadFIFO;
	mythreadFIFO = AfxBeginThread(// mythread为全局变量
		threadFIFO,//指向工作线程的控制函数
		this,//将要传送给控制函数的参数（this是CThreadDlg类的指针，才允许调用控件）
		THREAD_PRIORITY_NORMAL,//设置优先级
		0,//新线程使用的栈的以字节为单位的大小。如果为0，则缺省的栈大小与创建它的线程的栈大小相同
		0,//CREATE_SUSPENDED,//挂起（控制线程的创建过程）
		0//它指定了线程的安全特性。如果为空，将使用与创建它的线程相同的安全特性
		);
	/*CProgressCtrl *pProgressCtrl = (CProgressCtrl*)this->GetDlgItem(IDC_PROGRESS1);*///控制进度条
	/*pProgressCtrl->SetPos(10);*/
	//LRU
	mythreadFIFO = AfxBeginThread(
		threadLRU,
		this,
		THREAD_PRIORITY_NORMAL,
		0,0,0);


	//this->GetDlgItem(IDC_EDIT2)->GetWindowText(str);//得到页面驻留个数str

	// TODO:  在此添加控件通知处理程序代码
	//CDialogEx::OnOK();关闭对话框
}


void CPage_SMDlg::OnEnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CPage_SMDlg::OnBnClickedButton2()//保存
{
	//CString strText(_T(""));
	//GetDlgItemText(IDC_LIST2, strText);
	//CStdioFile  file;
	////HANDLE hOpenFile = (HANDLE)CreateFile(L"E:\\操作系统\\三级项目\\a.text", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
	//file.Open(_T("a.txt"), CFile::modeReadWrite);//modeCreate | CFile::modeWrite |typeText	
	//file.WriteString(strText);//, sizeof(strText));//把符串s写入文件
	//file.Close();  //关闭文件
	// TODO:  在此添加控件通知处理程序代码

	/*freopen("a.txt", "w", stdout);
	cout<<strText;*/
	CString str1;
	this->GetDlgItem(IDC_LIST2)->GetWindowText(str1);
	m_list3.AddString(str1);
	CStdioFile file;
	file.Open(_T(".\\a.txt"),CFile::modeNoTruncate | CFile::modeWrite);
	file.SeekToEnd();
	file.WriteString(str1);
	file.WriteString("\r\n");
	file.Flush();
	file.Close();
}

void CPage_SMDlg::OnBnClickedButton4()//继续
{
	ResumeThread(mythreadURL->m_hThread);
	/*CString a;
	a.Format(_T("%d"), K_time);
	this->GetDlgItem(IDC_EDIT1)->SetWindowText(a);*/

	//((CListBox*)GetDlgItem(IDC_LIST3))->SetWindowText(psc[j]);
	// TODO:  在此添加控件通知处理程序代码
}


void CPage_SMDlg::OnBnClickedButton3()//全部暂停
{
	SuspendThread(mythreadURL->m_hThread);//m_hThread当前线程的句柄
	// TODO:  在此添加控件通知处理程序代码
}


void CPage_SMDlg::OnEnKillfocusEdit2()//页面驻留个数事件
{
	CEdit* pEdit;
	CString size;
	this->GetDlgItem(IDC_EDIT2)->GetWindowText(size);
	m_size = _ttoi(size);
	if (m_size<=0||m_size>=10)
	{
		MessageBox( _T("请重新输入范围1至10的整数值"));
		CEdit *pEdit = (CEdit*)GetDlgItem(IDC_EDIT2);
		pEdit->SetFocus();
		pEdit->SetSel(0, size.GetLength());
	}
	
	// TODO:  在此添加控件通知处理程序代码
}


void CPage_SMDlg::OnEnKillfocusEdit3()//内存读取时间事件
{
	CEdit* pEdit;
	CString mtime;
	this->GetDlgItem(IDC_EDIT3)->GetWindowText(mtime);
	M_time = _ttoi(mtime);
	if (M_time <= 0 || M_time >= 1000)
	{
		MessageBox(_T("请重新输入范围1至1000的整数值"));
		CEdit *pEdit = (CEdit*)GetDlgItem(IDC_EDIT3);
		pEdit->SetFocus();
		pEdit->SetSel(0, mtime.GetLength());
	}

	// TODO:  在此添加控件通知处理程序代码
}


void CPage_SMDlg::OnEnKillfocusEdit4()//缺页中断时间事件
{
	CEdit* pEdit;
	CString qtime;
	this->GetDlgItem(IDC_EDIT4)->GetWindowText(qtime);
	Q_time = _ttoi(qtime);
	if (Q_time <= 0 || Q_time >= 1000)
	{
		MessageBox(_T("请重新输入范围1至1000的整数值"));
		CEdit *pEdit = (CEdit*)GetDlgItem(IDC_EDIT4);
		pEdit->SetFocus();
		pEdit->SetSel(0, qtime.GetLength());
	}

	// TODO:  在此添加控件通知处理程序代码
}


void CPage_SMDlg::OnEnKillfocusEdit5()//快表时间
{
	CEdit* pEdit;
	CString ktime;
	this->GetDlgItem(IDC_EDIT5)->GetWindowText(ktime);
	K_time = _ttoi(ktime);
	if (K_time <= 0 || K_time >= 10)
	{
		MessageBox(_T("请重新输入范围1至10的整数值"));
		CEdit *pEdit = (CEdit*)GetDlgItem(IDC_EDIT5);
		pEdit->SetFocus();
		pEdit->SetSel(0, ktime.GetLength());
	}
	// TODO:  在此添加控件通知处理程序代码
}
//格式转化：
//CString number;
//CString number2;
//CString number3;
//int numb[10];
//int numb2;
//
//this->GetDlgItem(IDC_EDIT1)->GetWindowText(number);
//numb2 = _ttoi(number);//整体转化正确
//for (int i = 0; i < 10; i++)
//{
//	char *c = new char[1];
//	c[0] = number.GetAt(i);
//	CString cs = c;
//	numb[i] = atoi(c);
//	number2.Format(_T("%d"), numb[i]);
//	m_list3.InsertString(i, number2);//InsertString和AddString不同
//}
//number3.Format(_T("%d"), numb2);
//this->GetDlgItem(IDC_EDIT2)->SetWindowText(number);
//this->GetDlgItem(IDC_EDIT4)->SetWindowText(number3);