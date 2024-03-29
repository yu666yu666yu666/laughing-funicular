/*服装销售管理
服装销售系统的设计与实现
    包含用户：管理员、销售员；
（1）管理员功能：查看、添加、修改、删除商品信息；
（2）销售员功能：商品浏览、查找、出售商品。
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct Product {
	char name[20]; //名字
	char color[20]; //颜色
    char style[20]; //类型风格
	double price; // 价格
	int inventory; // 库存
	struct Product *next;
};

void menu();
void login();
void menu_person();
void menu_admin();
void load_menu();
struct Product *load(const char *filename);
void showall();
void sale();
void search_menu();
struct Product *search(struct Product *head, const char *name);
void add_menu();
struct Product *add(struct Product *head, char *name,  char *color,  char *style,  double price,  int inventory);
void del_menu();
struct Product *del(struct Product *head, const char *name, int *isok);
void edit_menu();
struct Product *edit_name(struct Product *head, struct Product *temp);
struct Product *edit_color(struct Product *head, struct Product *temp);
struct Product *edit_style(struct Product *head, struct Product *temp);
struct Product *edit_price(struct Product *head, struct Product *temp);
struct Product *edit_inventory(struct Product *head, struct Product *temp);
void save(struct Product *head, const char *filename);

int main(void) {
	menu();
	return 0;
}

void menu() {
	int i;

	printf("Clothing sales Managment System[version: 38]\n");
	printf("yu\n\n\n");
	//菜单界面
	printf("=============================================================================\n");
	printf("1:> Administrator.\n");
	printf("2:> Person.\n");
	printf("0:> Exit.\n");
	printf("Please enter:");
	fflush(stdin);
	scanf("%d", &i);

	//选择功能
	switch (i) {
		//去往管理员登录界面
		case 1:
			login();
			break;
		//去往销售员界面
		case 2:
			menu_person();
			break;
		//退出程序		
		case 0:
			exit(1);
		default:
			break;
	}
}

void login() {
	char user[20];
	char pw[20];
	// count为密码尝试次数
	int count = 0, i = 0;

	//管理员账户密码设定
	char user1[20] = "yu";
	char pw1[20] = "666";

	//登录界面
	do {
		//输入用户名和密码
		printf("=============================================================================\n");
		printf("Clothing sales Managment System\n");
		printf("=============================================================================\n");
		printf("【Admin Login】\n");
		printf("User:");
		fflush(stdin);
		scanf("%s", user);
		printf("Password:");
		fflush(stdin);

		//输入密码时隐藏
		while ((pw[i] = getch()) != '\r') {
			if (pw[i] == '\b') {
				printf("\b \b");
				i--;
			}
			else {
				printf("*");
				i++;
			}
		}

		pw[i] = '\0';
		printf("\n");

		//判断用户名和密码是否正确，正确跳转管理员界面
		if (strcmp(user, user1) == 0 && strcmp(pw, pw1) == 0) {
			menu_admin();
			break;
		}
		//错误重试
		else {
			printf("ACCESS DENIED!\n");
			count++;
			system("pause");
		}

		if (count >= 3) {//尝试次数超过三次，退出程序 
		
			printf("Program terminated!\n");
			exit(2);
		}
	} while (1);//无特殊情况一直循环
}

void menu_admin() {
	int i;

	//判断文件存在与否
	load_menu();

	//菜单界面
	do {
		printf("=============================================================================\n");
		printf("Clothing sales Managment System:>Administrator\n");
		printf("1:> Show Product List.\n");
		printf("2:> Add New Product.\n");
		printf("3:> Delete Product.\n");
		printf("4:> Edit Product info.\n");
		printf("5:> Search Product.\n");
		printf("0:> Exit.\n");
		printf("Please enter:");
		fflush(stdin);
		scanf("%d", &i);

		//选择功能
		switch (i) {
			//显示商品列表
			case 1:
				showall();
				system("pause");
				break;
			//跳转至插入添加界面
			case 2:
				add_menu();
				system("pause");
				break;
			//跳转至删除界面
			case 3:
				del_menu();
				system("pause");
				break;
			//跳转至修改菜单
			case 4:
				edit_menu();
				system("pause");
				break;
			//跳转至搜索界面
			case 5:
				search_menu();
				system("pause");
				break;
			//退出程序
			case 0:
				exit(4);
			default:
				break;
		}
	} while (1);//无特殊情况一直循环
}

void menu_person() {
	int i;

	//判断文件存在与否
	load_menu();

	//菜单界面
	do {
		printf("=============================================================================\n");
		printf("Clothing sales Managment System:>Nomal\n");
		printf("1:> Search Product.\n");
		printf("2:> Show Product List.\n");
		printf("3:> Sale Product.\n");
		printf("0:> Exit.\n");
		printf("Please enter:");
		fflush(stdin);
		scanf("%d", &i);

		//选择功能
		switch (i) {
			//跳转到搜索界面
			case 1:
				search_menu();
				system("pause");
				break;
			//执行全部输出
			case 2:
				showall();
				system("pause");
				break;
			//销售员售卖
			case 3:
				sale();
				system("pause");
				break;
			//退出程序
			case 0:
				exit(3); 
			default:
				break;
		}
	} while (1);//无特殊情况一直循环
}

void load_menu() {
	struct Product* head = NULL;
	char temp[2];

	//文件存在，输出提示语句
	if (load("data.txt") != NULL) {
		printf("File Loaded!\n");
	}
	//文件不存在
	else {
		printf("FILE DON'T EXIST!\n");
		printf("Do you want to create file NOW?(y/n)\n");
		fflush(stdin);
		scanf("%s", temp);

		//是，新建文件
		if (strcmp("Y", temp) == 0 || strcmp("y", temp) == 0) {
			fopen("data.txt", "w");
			printf("FILE CREATED!\n");
		}
		//否，退出程序
		else {
			printf("NO FILE LOADED!\n");
			printf("Program Terminated!\n");
			exit(5);
		}
	}
}

//加载文件
struct Product* load(const char* filename) {
	FILE* fp = fopen(filename, "r");

	//文件不存在，返回 NULL
	if (fp == NULL) {
		return NULL;
	}

	//文件存在，依次录入链表，返回头节点 head
	struct Product buf, * head = NULL;

	while (fscanf(fp, "%s %s %s %lf %d", buf.name, buf.color, buf.style, &buf.price, &buf.inventory) > 0) {
		head = add(head, buf.name, buf.color, buf.style, buf.price, buf.inventory);
	}
	return head;
}

void showall() {
	struct Product* head = NULL, * p = NULL;

	//加载文件
		head = load("data.txt");
		//说明语句
		printf("=============================================================================\n");
		printf("Clothing sales Managment System:>ShowALL\n");
		printf("-----------Result---------\n");

		//循环输出
		for (p = head; p != NULL; p = p->next) {
			printf("Name:%s Color:%s Style:%s Price:%lf Inventory:%d\n", p->name, p->color, p->style, p->price, p->inventory);
		}
}

void add_menu() {
	struct Product* head = NULL;
	char temp1[2];
	char name[20];
	char color[20]; 
	char style[20];
	double price;
	int inventory;

	//加载文件
		head = load("data.txt");
		//提示及说明语句
		printf("=============================================================================\n");
		printf("Clothing sales Managment System\\Administrator\\AddNewProduct:>\n");
		printf("Please Enter:\n");

		//添加项目，按y继续操作
		do {
			printf("Product Name?\n");
			fflush(stdin);
			scanf("%s", name);
			printf("Color?\n");
			fflush(stdin);
			scanf("%s", color);
			printf("Style?\n");
			fflush(stdin);
			scanf("%s", style);
			printf("Price?\n");
			fflush(stdin);
			scanf("%lf", &price);
			printf("Inventory?\n");
			fflush(stdin);
			scanf("%d", &inventory);
			//调用插入添加函数
			head = add(head, name, color, style, price, inventory);
			printf("Countinue?(y/n)\n");
			fflush(stdin);
			scanf("%s", temp1);

		} while (strcmp("Y\0", temp1) == 0 || strcmp("y\0", temp1) == 0);

		//保存到文件
		save(head, "data.txt");
}
// 向链表以 name 的字典序插入数据，返回值为链表的头节点
struct Product* add(struct Product* head, char* name, char* color, char* style, double price,  int inventory) {
	// cur 为新增节点
	struct Product* cur = head, * prev = NULL;

	// 第一种情况，当 head == NULL 时，链表还没有创建，创建新链表
	if (cur == NULL) {
		head = cur = calloc(1, sizeof(struct Product));
	}
	else {
		// 找到待插入节点的后一个节点 cur 和前一个节点 prev
		for (; cur != NULL; cur = cur->next) {
			if (strcmp(name, cur->name) < 0) {
				break;
			}

			prev = cur;
		}

		// 第二种情况，当第一个节点比要插入节点大，待插入节点变为头节点
		if (prev == NULL) {
			struct Product* new_head = calloc(1, sizeof(struct Product));
			new_head->next = head;
			cur = head = new_head;
		}

		else {
			// 第三种情况，当要插入的节点位于中间
			if (prev != NULL && cur != NULL) {
				struct Product* new_node = calloc(1, sizeof(struct Product));
				prev->next = new_node;
				new_node->next = cur;
				cur = new_node;
			}
			// 第四种情况，要插入的节点为尾部，直接赋值原尾节点的next
			else {
				struct Product* new_tail = calloc(1, sizeof(struct Product));
				prev->next = new_tail;
				cur = new_tail;
			}
		}
	}

	// 开始赋值，填数据
	strcpy(cur->name, name);
	strcpy(cur->color, color);
	strcpy(cur->style, style);
	cur->price = price;
	cur->inventory = inventory;

	return head;
}

void del_menu() {
	struct Product* head = NULL;
	char name[20];
	int ok;
	
	//加载文件
	head = load("data.txt");
	//提示及说明语句
	printf("Please enter name of Product you want to Delete:");
	fflush(stdin);
	scanf("%s", name);
	head = del(head, name, &ok);

	//ok = 1，删除成功
	if (ok) {
		;
	}
	//ok = 0, 删除失败，说明项目不存在，输出提示语句
	else {
		printf("FILE DON'T EXIST!\n");
	}
	//保存到文件
	save(head, "data.txt");
}

struct Product* del(struct Product* head, const char* name, int *isok) {
	// cur 为要删除的节点
	struct Product* cur = head, * prev = NULL;

	// 找到待删除节点 cur 和前一个节点 prev
	for (; cur != NULL; cur = cur->next) {
		if (strcmp(name, cur->name) == 0) {
			break;
		}
		prev = cur;
	}

	//项目不存在，删除失败，返回头指针
	if (cur == NULL) {
		*isok = 0;
		return head;
	}

	// 第一种情况，要删除的是第一个节点，返回新的头指针
	if (prev == NULL) {
		struct Product* new_head = cur->next;
		free(cur);
		return new_head;
	}
	// 第二种情况，要删除的节点位于中间或尾部
	else {
		prev->next = cur->next;
		free(cur);
	}

	//上述两种情况说明成功删除
	*isok = 1;
	return head;
}

void edit_menu() {
	struct Product* head = NULL, *temp = NULL;
	char name[20];
	int i;

	//读取文件

		head = load("data.txt");
		//说明及提示语句
		printf("=============================================================================\n");
		printf("Clothing sales Managment System:>Edit\n");
		printf("Please enter name of Product you want to Edit:");
		fflush(stdin);
		scanf("%s", name);
		//搜索返回值为 NULL ，项目不存在，跳转管理员菜单
		if (search(head, name) == NULL) {
			printf("ITEM DON'T EXSIT!\n");
			system("pause");
			menu_admin();
		}
		//搜索返回值为项目对应节点指针，进行操作
		else {
			temp = search(head, name);
			//提示及说明语句
			printf("Clothing sales Managment System\\Edit\\%s:>\n", temp->name);
			printf("1:> Edit Product Name\n");
			printf("2:> Edit Product Color\n");
			printf("3:> Edit Product Style\n");
			printf("4:> Edit Product Price\n");
			printf("5:> Edit Product inventory\n");
			printf("0:> Return to Menu\n");
			printf("Please enter:");
			fflush(stdin);
			scanf("%d", &i);

			//选择功能
			switch (i) {
				//跳转商品名修改界面
				case 1:
					head = edit_name(head, temp);
					break;
				//跳转颜色修改界面
				case 2:
					head = edit_color(head, temp);
					break;
				//跳转类型修改界面
				case 3:
					head = edit_style(head, temp);
					break;
				//跳转价格修改界面
				case 4:
					head = edit_price(head, temp);
					break;
				//跳转库存修改界面
				case 5:
					head = edit_inventory(head, temp);
					break;
				case 6:
					exit(6);
				default:
					break;
			}
			//保存到文件
			save(head, "data.txt");
		}
}

//修改名称。删除原节点，插入临时节点
struct Product* edit_name(struct Product* head, struct Product* temp) { 
	int ok;
	char name[20];
	char color[20];
	char style[20];
	double price;
	int inventory;

	printf("Please enter Name to replace %s:\n", temp->name);
	//将原信息复制到临时变量上
	strcpy(name, temp->name);
	strcpy(color, temp->color);
	strcpy(style, temp->style);
	price = temp->price;
	inventory = temp->inventory;
	//删除原节点
	head = del(head, temp->name, &ok);
	fflush(stdin);
	scanf("%s", name);
	//插入临时节点
	head = add(head, name, color, style, price, inventory);
	return head;
}

//修改颜色。
struct Product* edit_color(struct Product* head, struct Product* temp) { 
	int ok;
	char name[20];
	char color[20];
	char style[20];
	double price;
	int inventory;

	printf("Please enter Color to replace %s:\n", temp->color);
	//将原信息复制到临时变量上
	strcpy(name, temp->name);
	strcpy(color, temp->color);
	strcpy(style, temp->style);
	price = temp->price;
	inventory = temp->inventory;
	//删除原节点
	head = del(head, temp->name, &ok);
	fflush(stdin);
	scanf("%s", color);
	//插入临时节点
	head = add(head, name, color, style, price, inventory);
	return head;
}

//修改类型
struct Product* edit_style(struct Product* head, struct Product* temp) { 
	int ok;
	char name[20];
	char color[20];
	char style[20];
	double price;
	int inventory;

	printf("Please enter Style to replace %s:\n", temp->style);
	//将原信息复制到临时变量上
	strcpy(name, temp->name);
	strcpy(color, temp->color);
	strcpy(style, temp->style);
	price = temp->price;
	inventory = temp->inventory;
	//删除原节点
	head = del(head, temp->name, &ok);
	fflush(stdin);
	scanf("%s", style);
	//插入临时节点
	head = add(head, name, color, style, price, inventory);
	return head;
}

//修改库存
struct Product* edit_inventory(struct Product* head, struct Product* temp) {
	printf("Inventory?\n");
	printf("Please enter Inventory to replace %d:\n", temp->inventory);
	fflush(stdin);
	scanf("%d", &temp->inventory);
	return head;
}

//修改价格
struct Product* edit_price(struct Product* head, struct Product* temp) {
	printf("Price?\n");
	printf("Please enter Price to replace %lf:\n", temp->price);
	fflush(stdin);
	scanf("%lf", &temp->price);
	return head;
}

void search_menu() {
	struct Product* head = NULL, * temp = NULL;
	char name[30];

	//加载文件
		head = load("data.txt");
		//提示及说明语句
		printf("=============================================================================\n");
		printf("Clothing sales Managment System:>Search\n");
		printf("Please enter name of Product you want to Search:");
		fflush(stdin);
		scanf("%s", name);

		//返回值为 NULL ，项目不存在，输出提示语句
		if (search(head, name) == NULL) {
			printf("ITEM NOT EXIST!");
		}
		//返回值为项目对应节点指针，输出相关信息
		else {
			temp = search(head, name);
			printf("Name:%s Color:%s Style:%s Price:%lf Inventory:%d\n", temp->name, temp->color, temp->style, temp->price, temp->inventory);
		}
}

// 搜索节点，找到返回该节点指针，没有找到返回空
struct Product* search(struct Product* head, const char* name) {
	for (struct Product* cur = head; cur != NULL; cur = cur->next)
	{
		if (strcmp(name, cur->name) == 0)
		{
			return cur;
		}
	}
	return NULL;
}

void sale() {
	struct Product* head = NULL, *temp = NULL;
	char name[20];
	int i;

	//读取文件

		head = load("data.txt");
		//说明及提示语句
		printf("=============================================================================\n");
		printf("Clothing sales Managment System:>Sale\n");
		printf("Please enter name of Product you Sale:");
		fflush(stdin);
		scanf("%s", name);
		//搜索返回值为 NULL ，项目不存在，跳转管理员菜单
		if (search(head, name) == NULL) {
			printf("ITEM DON'T EXSIT!\n");
			system("pause");
			menu_admin();
		}
		//搜索返回值为项目对应节点指针，进行操作
		else {
			temp = search(head, name);
			printf("Please enter the quantity you are selling\n");
			fflush(stdin);
			scanf("%d", &i);
			temp->inventory-=i;
			//保存到文件
			save(head, "data.txt");
		}
}

void save(struct Product* head, const char* filename) {
	FILE* fp = fopen(filename, "w");

	for (struct Product* cur = head; cur != NULL; cur = cur->next)
	{
		fprintf(fp, "%s %d %lf %d\n", cur->name, cur->color , cur->style, cur->price, cur->inventory);
	}
	fclose(fp);
}
