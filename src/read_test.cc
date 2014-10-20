#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <string>
struct packet
{
	int length;
	char *content;
	int type;
	struct packet * next;
};
typedef packet packet,*p_packet;
void save_packet(packet *&head,std::string  *&script)
{
	int len=script->length();//前面部分str
	char *content=(char *)malloc(len+1);
	memset(content,'\0',len+1);
	strcpy(content,script->c_str());
	p_packet p=(p_packet)malloc(sizeof(packet));	
	p->length=len;
	p->content=content;
	p->type=1;
	p->next=NULL;
	head->next=p;
	head=p;
	delete script;
	script=NULL;
	script=new std::string();
}
int main()
{
	FILE *file=fopen("./test.txt","r");
	std::string *script=new std::string();
	p_packet head=(p_packet)malloc(sizeof(packet));
	head->length=0;
	head->content=NULL;
	head->type=-1;
	head->next=NULL;
	p_packet header=head;
	while(feof(file)==0)
	{
		char buffer[1024]={'\0'};
		//int length=0;
		//getline(buffer,&length);
		size_t length=fread(buffer,1,1024,file);
		int i=0;
		for(i=0;i<length;i++)
		{
			if(buffer[i]=='<'&&buffer[i+1]=='?'&&buffer[i+2]=='j'&&buffer[i+3]=='s')
			{
				if(buffer[i+4]==32||buffer[i+4]=='\n'||(buffer[i+4]=='\r'&&buffer[i+5]=='\n'))//判断换行符\n,windows \r\n,以及空白
				{
					if(script!=NULL)
					{
						save_packet(head,script);
						if(buffer[i+4]=='\r'&&buffer[i+5]=='\n')
							i=i+5;
						else
							i=i+4;
					}
				}
			}
			if(buffer[i]=='?'&&buffer[i+1]=='>')
			{
				save_packet(head,script);
				i+=2;
			}
			char temp=*(buffer+i);
			script->append(&temp);
		}
	}
	save_packet(head,script);
	return 0;
}
