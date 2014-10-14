#include "ftpproto.h"
#include "sysutil.h"
#include "str.h"
#include "ftpcodes.h"
#include "tunable.h"
#include "privsock.h"

void ftp_reply(session_t *sess, int status, const char *text);
void ftp_lreply(session_t *sess, int status, const char *text);

void handle_alarm_timeout(int sig);
void handle_sigalrm(int sig);
void handler_sigurg(int sig);
void start_cmdio_alarm(void);
void start_data_alarm(void);

int list_common(session_t *sess, int detail);
void limit_rate(session_t *sess, int bytes_transfered, int is_upload);
void upload_common(session_t *sess, int is_append);

void check_abor(session_t *sess);

int get_port_fd(session_t *sess);
int get_pasv_fd(session_t *sess);
int get_transfer_fd(session_t *sess);
int port_active(session_t *sess);
int pasv_active(session_t *sess);


//ֻ���ڱ�ģ�鵱�У���static�����κ���
static void do_user(session_t *sess);
static void do_pass(session_t *sess);
static void do_cwd(session_t *sess);
static void do_cdup(session_t *sess);
static void do_quit(session_t *sess);
static void do_port(session_t *sess);
static void do_pasv(session_t *sess);
static void do_type(session_t *sess);
/*
static void do_stru(session_t *sess);
static void do_mode(session_t *sess);
*/
static void do_retr(session_t *sess);
static void do_stor(session_t *sess);
static void do_appe(session_t *sess);
static void do_list(session_t *sess);
static void do_nlst(session_t *sess);
static void do_rest(session_t *sess);
static void do_abor(session_t *sess);
static void do_pwd(session_t *sess);
static void do_mkd(session_t *sess);
static void do_rmd(session_t *sess);
static void do_dele(session_t *sess);
static void do_rnfr(session_t *sess);
static void do_rnto(session_t *sess);
static void do_site(session_t *sess);
static void do_syst(session_t *sess);
static void do_feat(session_t *sess);
static void do_size(session_t *sess);
static void do_stat(session_t *sess);
static void do_noop(session_t *sess);
static void do_help(session_t *sess);

static void do_site_chmod(session_t *sess, char *chmod_arg);
static void do_site_umask(session_t *sess, char *umask_arg);

typedef struct ftpcmd
{
	const char *cmd;
	void (*cmd_handler)(session_t *sess);
} ftpcmd_t;

static ftpcmd_t ctrl_cmds[] = {
	/* ���ʿ������� */
	{"USER",	do_user	},
	{"PASS",	do_pass	},
	{"CWD",		do_cwd	},
	{"XCWD",	do_cwd	},
	{"CDUP",	do_cdup	},
	{"XCUP",	do_cdup	},
	{"QUIT",	do_quit	},
	{"ACCT",	NULL	},
	{"SMNT",	NULL	},
	{"REIN",	NULL	},
	/* ����������� */
	{"PORT",	do_port	},
	{"PASV",	do_pasv	},
	{"TYPE",	do_type	},
	{"STRU",	NULL/*do_stru*/	},
	{"MODE",	NULL/*do_mode*/ },

	/* �������� */
	{"RETR",	do_retr	},
	{"STOR",	do_stor	},
	{"APPE",	do_appe	},
	{"LIST",	do_list	},
	{"NLST",	do_nlst	},
	{"REST",	do_rest	},
	{"ABOR",	do_abor	},
	{"\377\364\377\362ABOR", do_abor},
	{"PWD",		do_pwd	},
	{"XPWD",	do_pwd	},
	{"MKD",		do_mkd	},
	{"XMKD",	do_mkd	},
	{"RMD",		do_rmd	},
	{"XRMD",	do_rmd	},
	{"DELE",	do_dele	},
	{"RNFR",	do_rnfr	},
	{"RNTO",	do_rnto	},
	{"SITE",	do_site	},
	{"SYST",	do_syst	},
	{"FEAT",	do_feat },
	{"SIZE",	do_size	},
	{"STAT",	do_stat	},
	{"NOOP",	do_noop	},
	{"HELP",	do_help	},
	{"STOU",	NULL	},
	{"ALLO",	NULL	}
};

session_t *p_sess;

void handle_alarm_timeout(int sig)
{
	shutdown(p_sess->ctrl_fd, SHUT_RD);
	ftp_reply(p_sess, FTP_IDLE_TIMEOUT, "Timeout");
	shutdown(p_sess->ctrl_fd, SHUT_WR);
	exit(EXIT_FAILURE);
}

void handle_sigalrm(int sig)
{
	
	if (!p_sess->data_process)
	{
		ftp_reply(p_sess, FTP_DATA_TIMEOUT, "Data timeout. Reconnect. Sorry.");
		exit(EXIT_FAILURE);
	}
	
	//���򣬵�ǰ�������ݴ����״̬�յ��˳�ʱ�ź�
	p_sess->data_process = 0;
	start_data_alarm();
}

void handler_sigurg(int sig)
{
	if (p_sess->data_fd == -1)
	{
		return;
	}

	char cmdline[MAX_COMMAND_LINE] = {0};
	int ret = readline(p_sess->ctrl_fd, cmdline, MAX_COMMAND_LINE);
	if (ret <= 0)
	{
		ERR_EXIT("readline");
	}
	
	str_trim_crlf(cmdline);
	str_upper(cmdline);

	if (strcmp(cmdline, "ABOR") == 0
		|| strcmp(cmdline, "\377\364\377\362ABOR") == 0)
	{
		p_sess->abor_received = 1;
		shutdown(p_sess->data_fd, SHUT_RDWR);
	}
	else
	{
		ftp_reply(p_sess, FTP_BADCMD, "Unknown command.");
	}
}

void check_abor(session_t *sess)
{
	if (sess->abor_received)
	{
		sess->abor_received = 0;
		ftp_reply(p_sess, FTP_ABOROK, "ABOR successful.");
	}
}

void start_cmdio_alarm(void)
{
	if (tunable_idle_session_timeout > 0)
	{
		//��װ�ź�
		signal(SIGALRM, handle_alarm_timeout);
		//��װ����
		alarm(tunable_idle_session_timeout);
	}
}

void start_data_alarm(void)
{
	if (tunable_data_connection_timeout > 0)
	{
		//��װ�ź�
		signal(SIGALRM, handle_sigalrm);
		//��װ����
		alarm(tunable_data_connection_timeout);
	}
	else if (tunable_idle_session_timeout > 0)
	{
		//�ر���ǰ��װ������
		alarm(0);
	}
}

void handle_child(session_t *sess)
{
	//writen(sess->ctrl_fd, "220 (miniftpd 0.1)\r\n", strlen("220 (miniftpd 0.1)\r\n"));
	ftp_reply(sess, FTP_GREET, "(miniftpd 0.1)");
	int ret;
	while (1)
	{
		memset(sess->cmdline, 0, sizeof(sess->cmdline));
		memset(sess->cmd, 0, sizeof(sess->cmd));
		memset(sess->arg, 0, sizeof(sess->arg));

		start_cmdio_alarm();
		ret = readline(sess->ctrl_fd, sess->cmdline, MAX_COMMAND_LINE);
		if (ret == -1)
			ERR_EXIT("readline");
		else if (ret == 0)
			exit(EXIT_SUCCESS);

		printf("cmdline=[%s]\n", sess->cmdline);
		//ȥ��\r\n
		str_trim_crlf(sess->cmdline);
		printf("cmdline=[%s]\n", sess->cmdline);
		//����FTP���������
		str_split(sess->cmdline, sess->cmd, sess->arg, ' ');
		printf("cmd=[%s] arg=[%s]\n", sess->cmd, sess->arg);
		//������ת���ɴ�
		str_upper(sess->cmd);
		//printf("cmd=[%s]\n", sess->cmd);
		//����FTP����
		/*
		if (strcmp("USER", sess->cmd) == 0)
		{
			do_user(sess);
		}
		else if (strcmp("PASS", sess->cmd) == 0)
		{
			do_pass(sess);
		}
		*/

		int i;
		int size = sizeof(ctrl_cmds) / sizeof(ctrl_cmds[1]);
		for (i=0; i<size; i++)
		{
			if (strcmp(ctrl_cmds[i].cmd, sess->cmd) == 0)
			{
				if (ctrl_cmds[i].cmd_handler != NULL)
				{
					ctrl_cmds[i].cmd_handler(sess);
				}
				else
				{
					ftp_reply(sess, FTP_COMMANDNOTIMPL, "Unimplement command.");
				}

				break;
			}
		}

		if (i == size)
		{
			ftp_reply(sess, FTP_BADCMD, "Unknown commad.");
		}
	}

}

void ftp_reply(session_t *sess, int status, const char *text)
{
	char buf[1024] = {0};
	sprintf(buf, "%d %s\r\n", status, text);
	writen(sess->ctrl_fd, buf, strlen(buf));
}

void ftp_lreply(session_t *sess, int status, const char *text)
{
	char buf[1024] = {0};
	sprintf(buf, "%d-%s\r\n", status, text);
	writen(sess->ctrl_fd, buf, strlen(buf));
}

int list_common(session_t *sess, int detail)
{
	DIR *dir = opendir(".");
	if (dir == NULL)
	{
		return 0;
	}

	struct dirent *dt;
	struct stat sbuf;
	while ((dt = readdir(dir)) != NULL)
	{	
		if (lstat(dt->d_name, &sbuf) < 0)
			continue;

		if (dt->d_name[0] == '.')
			continue;

		char buf[1024] = {0};
		if (detail)
		{
			const char *perms = statbuf_get_perms(&sbuf);
			
			int off = 0;
			off += sprintf(buf, "%s", perms);
			off += sprintf(buf + off, " %3d %-8d %-8d", sbuf.st_nlink, sbuf.st_uid, sbuf.st_gid);
			off += sprintf(buf + off, "%8lu ", (unsigned long)sbuf.st_size);


			const char *datebuf = statbuf_get_date(&sbuf);

			off += sprintf(buf + off, "%s ", datebuf);

			if (S_ISLNK(sbuf.st_mode))
			{
				char tmp[1024] = {0};
				readlink(dt->d_name, tmp, sizeof(tmp));
				off += sprintf(buf + off, "%s -> %s \r\n", dt->d_name, tmp);
			}
			else
			{
				sprintf(buf + off, "%s\r\n", dt->d_name);
			}
			
		}
		else 
		{
			sprintf(buf, "%s\r\n", dt->d_name);
		}

		//printf("%s", buf);
		writen(sess->data_fd, buf, strlen(buf));
	}

	closedir(dir);

	return 1;
}

void limit_rate(session_t *sess, int bytes_transfered, int is_upload)
{
	sess->data_process = 1;
	// ˯��ʱ�� = (��ǰ�����ٶ� / ������ٶ� �C 1) * ��ǰ����ʱ��;
	long curr_sec = get_time_sec();
	long curr_usec = get_time_usec();

	double elapsed;
	elapsed = (double)(curr_sec - sess->bw_transfer_start_sec);
	elapsed += (double)(curr_usec - sess->bw_transfer_start_usec) / (double)1000000;

	if (elapsed <= (double)0)
	{
		elapsed = (double)0.01;
	}

	//���㵱ǰ�����ٶ�
	unsigned int bw_rate =  (unsigned int)((double)bytes_transfered / elapsed);

	double rate_ratio;
	if (is_upload)
	{
		if (bw_rate <= sess->bw_upload_rate_max)
		{
			//����Ҫ����
			sess->bw_transfer_start_sec = curr_sec;
			sess->bw_transfer_start_usec = curr_usec;
			return;
		}

		rate_ratio = bw_rate / sess->bw_upload_rate_max;
	}
	else
	{
		if (bw_rate <= sess->bw_download_rate_max)
		{
			//����Ҫ����
			sess->bw_transfer_start_sec = curr_sec;
			sess->bw_transfer_start_usec = curr_usec;
			return;
		}
		rate_ratio = bw_rate / sess->bw_download_rate_max;
	}

	double pause_time;
	pause_time = (rate_ratio - (double)1) * elapsed;

	nano_sleep(pause_time);				//����ʹ��sleep����Ϊsleep���������������������sleep�ڲ�ʹ��SIGALRM�ź���ʵ�֣�����alarm��sleep����ͬʱʹ�ã�alarmʵ�ֿ��жϿ�����

	sess->bw_transfer_start_sec = get_time_sec();
	sess->bw_transfer_start_usec = get_time_usec();
}

void upload_common(session_t *sess, int is_append)
{
		//������������
	if (get_transfer_fd(sess) == 0)
	{
		return;
	}
	long long offset = sess->restart_pos;
	sess->restart_pos = 0;

	int fd = open(sess->arg, O_CREAT | O_WRONLY, 0666);
	if (fd == -1)
	{
		ftp_reply(sess, FTP_UPLOADFAIL, "Could not create file.");
		return;
	}
	
	//��д��
	int ret;
	ret = lock_file_wirte(fd);
	if (ret  < 0)
	{
		ftp_reply(sess, FTP_UPLOADFAIL, "Could not create file.");
		return;
	}
	//STOR
	//STOR+REST
	//APPE
	if (!is_append && offset == 0)		//STOR
	{
		ftruncate(fd, 0);
		if (lseek(fd, 0, SEEK_SET) < 0)
		{
			ftp_reply(sess, FTP_UPLOADFAIL, "Could not create file.");
			return;
		}
	}
	else if (!is_append && offset != 0)		//STOR+REST
	{
		if (lseek(fd, offset, SEEK_SET) < 0)
		{
			ftp_reply(sess, FTP_UPLOADFAIL, "Could not create file.");
			return;
		}
	}
	else if (is_append)					//APPE
	{
		if (lseek(fd, 0, SEEK_END) < 0)
		{
			ftp_reply(sess, FTP_UPLOADFAIL, "Could not create file.");
			return;
		}
	}
	
	//150
	/*
	char text[1024] = {0};
	if (sess->is_ascii)
	{
		sprintf(text, "Opening ASCII mode data connection for %s (%lld bytes)",
			sess->arg, (long long)sbuf.st_size);
	}
	else
	{
		sprintf(text, "Opening BINARY mode data connection for %s (%lld bytes)",
			sess->arg, (long long)sbuf.st_size);
	}
	*/

	ftp_reply(sess, FTP_DATACONN, "Ok to send data.");

	int flag = 0;
	//�ϴ��ļ�
	
	char buf[4096] = {0};

	// ˯��ʱ�� = (��ǰ�����ٶ� / ������ٶ� �C 1) * ��ǰ����ʱ��;

	sess->bw_transfer_start_sec = get_time_sec();
	sess->bw_transfer_start_usec = get_time_usec(); 

	while (1)
	{
		ret = read(sess->data_fd, buf, sizeof(buf));
		if (ret == -1)
		{
			if (errno == EINTR)
			{
				continue;
			}
			else
			{
				flag = 2;
				break;
			}
		}
		else if (ret == 0)
		{
			flag = 0;
			break;
		}
		
		limit_rate(sess, ret, 1);
/*		if (sess->abor_received)
		{
			flag = 2;
			break;
		}
*/
		if (writen(fd, buf, ret) != ret)
		{
			flag = 1;
			break;
		}

		if (sess->abor_received)
		{
			flag = 2;
			break;
		}
		
	}

	//�ر������׽���
	close(sess->data_fd);
	sess->data_fd = -1;

	close(fd);
	if (flag == 0 && !sess->abor_received)
	{
		//226
		ftp_reply(sess, FTP_TRANSFEROK, "Transfer complete.");
	}
	else if (flag == 1)
	{
		//451
		ftp_reply(sess, FTP_BADSENDFILE, "Failure writing to local file.");
	}
	else if (flag == 2)
	{
		//426
		ftp_reply(sess, FTP_BADSENDNET, "Failure reading from network stream.");
	}

	check_abor(sess);

	//���¿�����������ͨ������
	start_cmdio_alarm();	
}

int port_active(session_t *sess)
{
	if (sess->port_addr)
	{
		if (pasv_active(sess))
		{
			fprintf(stderr, "botn port an pasv are active");
			exit(EXIT_FAILURE);
		}
		return 1;
	}

	return 0;
}

int pasv_active(session_t *sess)
{
	/*
	if (sess->pasv_listen_fd != -1)
	{
		if (port_active(sess))
		{
			fprintf(stderr, "botn port an pasv are active");
			exit(EXIT_FAILURE);
		}
		return 1;
	}
	*/
	priv_sock_send_cmd(sess->child_fd, PRIV_SOCK_PASV_ACTIVE);
	int active = priv_sock_get_int(sess->child_fd);
	if (active)
	{
		if (port_active(sess))
		{
			fprintf(stderr, "botn port an pasv are active");
			exit(EXIT_FAILURE);
		}
		return 1;
	}

	return 0;
}

int get_port_fd(session_t *sess)
{
	/*
	��nobody���̷���PRIV_SOCK_GET_DATA_SOCK����	1
	��nobody���̷�������port			4
	��nobody���̷����ַ���ip			������	
	*/
	priv_sock_send_cmd(sess->child_fd, PRIV_SOCK_GET_DATA_SOCK);
	unsigned short port = ntohs(sess->port_addr->sin_port);
	char *ip = inet_ntoa(sess->port_addr->sin_addr);
	priv_sock_send_int(sess->child_fd, (int)port);
	priv_sock_send_buf(sess->child_fd, ip, strlen(ip));
	
	char res = priv_sock_get_result(sess->child_fd);
	if (res == PRIV_SOCK_RESULT_BAD)
	{
		return 0;
	}
	else if (res == PRIV_SOCK_RESULT_OK)
	{
		sess->data_fd = priv_sock_recv_fd(sess->child_fd);
	}

	return 1;
}

int get_pasv_fd(session_t *sess)
{
	priv_sock_send_cmd(sess->child_fd, PRIV_SOCK_PASV_ACCEPT);
	char res = priv_sock_get_result(sess->child_fd);
	if (res == PRIV_SOCK_RESULT_BAD)
	{
		return 0;
	}
	else if (res == PRIV_SOCK_RESULT_OK)
	{
		sess->data_fd = priv_sock_recv_fd(sess->child_fd);
	}

	return 1;
}

int get_transfer_fd(session_t *sess)
{
	//����Ƿ��յ�PORT����PASV����
	if (!port_active(sess) && !pasv_active(sess))
	{
		ftp_reply(sess, FTP_BADSENDCONN, "Use PORT or PASV first.");
		return 0;
	}
	//���������ģʽ(port)
	int ret = 1;
	if (port_active(sess))
	{
		/*
		socket
		bind 20
		connet
		*/
		//tcp_cliet
		/*

		int fd = tcp_client(0);
		if (connect_timeout(fd, sess->port_addr, tunable_connect_timeout) < 0)
		{
			close(fd);
			return 0;
		}

		sess->data_fd = fd;
		*/
		if (get_port_fd(sess) == 0)
		{
			ret = 0;
		}
	}

	if (pasv_active(sess))
	{
		/*
		int fd = accept_timeout(sess->pasv_listen_fd, NULL, tunable_accept_timeout);
		close(sess->pasv_listen_fd);

		if (fd == -1)
		{
			return 0;
		}
		sess->data_fd = fd;
		*/
		if (get_pasv_fd(sess) == 0)
		{
			ret = 0;
		}
	}

	if (sess->port_addr)
	{
		free(sess->port_addr);
		sess->port_addr = NULL;
	}
	if (ret)
	{
		//���°�װSIGALRM�źţ�����������
		start_data_alarm();
	}
	return ret;
}

static void do_user(session_t *sess)
{
	//USER gary
	//�����û�������ȡ�����ļ�
	struct passwd *pw = getpwnam(sess->arg);
	if (pw == NULL)
	{
		//�û�������
		//writen(sess->ctrl_fd, "530 Login incorrect.\r\n", strlen("530 Login incorrect.\r\n"));
		ftp_reply(sess, FTP_LOGINERR, "Login incorrect.");
		return;
	}

	sess->uid = pw->pw_uid;
	//writen(sess->ctrl_fd, "331 Please specify the password.\r\n", strlen("331 Please specify the password.\r\n"));
	ftp_reply(sess, FTP_GIVEPWORD, "Please specify the password.");
}

static void do_pass(session_t *sess)
{
	//PASS 920517
	//����uid��ȡ�����ļ���Ϣ
	struct passwd *pw = getpwuid(sess->uid);
	if (pw == NULL)
	{
		//�û�������
		ftp_reply(sess, FTP_LOGINERR, "Login incorrect.");
		return;
	}

	printf("name=[%s]\n", pw->pw_name);
	struct spwd *sp = getspnam(pw->pw_name);
	if (sp == NULL)
	{
		ftp_reply(sess, FTP_LOGINERR, "Login incorrect.");
		return;
	}

	//������������м��� crypt(����1���ģ�����2���ӣ���Ӱ���ļ����Ѿ����ܵ�����)���ؼ��ܹ�������
	char *encrypted_pass = crypt(sess->arg, sp->sp_pwdp);
	//��֤����
	if (strcmp(encrypted_pass, sp->sp_pwdp) != 0)
	{
		ftp_reply(sess, FTP_LOGINERR, "Login incorrect.");
		return;
	}
	
	//ע�ᵱǰ���̿ɽ���SIGURG�źţ���������ǰ��������������SIGURG�źţ��ɽ��ܴ������ݣ��������ݣ�
	signal(SIGURG, handler_sigurg);
	activate_sigurg(sess->ctrl_fd);

	umask(tunable_local_umask);
	setegid(pw->pw_gid);
	seteuid(pw->pw_uid);
	chdir(pw->pw_dir);
	//writen(sess->ctrl_fd, "230 Login successful\r\n", strlen("230 Login successful\r\n"));
	ftp_reply(sess, FTP_LOGINOK, "Login successful");
}

static void do_cwd(session_t *sess)
{
	if (chdir(sess->arg))
	{
		ftp_reply(sess, FTP_FILEFAIL, "Failed to change directory.");
		return;
	}

	ftp_reply(sess, FTP_CWDOK, " Directory successfully changed.");
}

static void do_cdup(session_t *sess)
{
	if (chdir(".."))
	{
		ftp_reply(sess, FTP_FILEFAIL, "Failed to change directory.");
		return;
	}

	ftp_reply(sess, FTP_CWDOK, " Directory successfully changed.");

}

static void do_quit(session_t *sess)
{
	ftp_reply(sess, FTP_GOODBYE, "Goodbye.");
	exit(EXIT_SUCCESS);
}

static void do_port(session_t *sess)
{
	//PORT 192,168,126,1,139,65
	unsigned int v[6];
	
	sscanf(sess->arg, "%u,%u,%u,%u,%u,%u", &v[2], &v[3], &v[4], &v[5], &v[0], &v[1]); 
	sess->port_addr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
	memset(sess->port_addr, 0, sizeof(struct sockaddr_in));
	sess->port_addr->sin_family = AF_INET;
	unsigned char *p = (unsigned char*)&sess->port_addr->sin_port;
	p[0] = v[0];
	p[1] = v[1];

	p = (unsigned char*)&sess->port_addr->sin_addr;
	p[0] = v[2];
	p[1] = v[3];
	p[2] = v[4];
	p[3] = v[5];

	ftp_reply(sess, FTP_PORTOK, "PORT command successful. Consider using PASV.");

}

static void do_pasv(session_t *sess)
{
	//Entering Passive Mode (192,168,126,188,53,140).

	char ip[16] = {0};
	getlocalip(ip);
	/*
	sess->pasv_listen_fd = tcp_server(ip, 0);
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	if (getsockname(sess->pasv_listen_fd, (struct sockaddr*)&addr, &addrlen) < 0)
		ERR_EXIT("getsockname");

	unsigned short port = ntohs(addr.sin_port);
	*/

	priv_sock_send_cmd(sess->child_fd, PRIV_SOCK_PASV_LISTEN);
	unsigned short port = (int)priv_sock_get_int(sess->child_fd);

	unsigned int v[4];
	sscanf(ip, "%u.%u.%u.%u", &v[0], &v[1], &v[2], &v[3]);
	char text[1024] = {0};
	sprintf(text, "Entering Passive Mode (%u,%u,%u,%u,%u,%u)", 
		v[0], v[1], v[2], v[3], port>>8, port&0xFF);

	ftp_reply(sess, FTP_PASVOK, text);
}

static void do_type(session_t *sess)
{
	if (strcmp(sess->arg, "A") == 0)
	{
		sess->is_ascii = 1;
		ftp_reply(sess, FTP_TYPEOK, "Switching to ASCII mode.");
	}
	else if (strcmp(sess->arg, "I") == 0)
	{
		sess->is_ascii = 0;
		ftp_reply(sess, FTP_TYPEOK, "Switching to Binary mode.");
	}
	else
	{
		ftp_reply(sess, FTP_BADCMD,"Unrecognised TYPE command.");
	}

}
/*
static void do_stru(session_t *sess)
{
}

static void do_mode(session_t *sess)
{
}
*/
static void do_retr(session_t *sess)
{
	//�����ļ�
	//�ϵ�����

	//������������
	if (get_transfer_fd(sess) == 0)
	{
		return;
	}
	long long offset = sess->restart_pos;
	sess->restart_pos = 0;

	int fd = open(sess->arg, O_RDONLY);
	if (fd == -1)
	{
		ftp_reply(sess, FTP_FILEFAIL, "Failed to open file.");
		return;
	}
	
	//�Ӷ���
	int ret;
	ret = lock_file_read(fd);
	if (ret  < 0)
	{
		ftp_reply(sess, FTP_FILEFAIL, "Failed to open file.");
		return;
	}
	
	//�ж��Ƿ�����ͨ�ļ�
	struct stat sbuf;
	ret = fstat(fd, &sbuf);
	if (!S_ISREG(sbuf.st_mode))
	{
		ftp_reply(sess, FTP_FILEFAIL, "Failed to open file.");
		return;
	}
	
	if (offset != 0)
	{
		ret = lseek(fd, offset, SEEK_SET);
		if (ret == -1)
		{
			ftp_reply(sess, FTP_FILEFAIL, "Failed to open file.");
			return;
		}
	}

	//150 Opening ASCII mode data connection for /home/gary/tmp/1.txt (8 bytes).
	//150
	char text[1024] = {0};
	if (sess->is_ascii)
	{
		sprintf(text, "Opening ASCII mode data connection for %s (%lld bytes)",
			sess->arg, (long long)sbuf.st_size);
	}
	else
	{
		sprintf(text, "Opening BINARY mode data connection for %s (%lld bytes)",
			sess->arg, (long long)sbuf.st_size);
	}

	ftp_reply(sess, FTP_DATACONN, text);

	int flag = 0;
	//�����ļ�
	
	/*
	char buf[4096] = {0};
	while (1)
	{
		ret = read(fd, buf, sizeof(buf));
		if (ret == -1)
		{
			if (errno == EINTR)
			{
				continue;
			}
			else
			{
				flag = 1;
				break;
			}
		}
		else if (ret == 0)
		{
			flag = 0;
			break;
		}

		if (writen(sess->data_fd, buf, ret) != ret)
		{
			flag = 2;
			break;
		}
		
	}
	*/
	
	long long bytes_to_send = sbuf.st_size;
	if (offset > bytes_to_send)
	{
		bytes_to_send = 0;
	}
	else
	{
		bytes_to_send -= offset;
	}

	sess->bw_transfer_start_sec = get_time_sec();
	sess->bw_transfer_start_usec = get_time_usec();
	while (bytes_to_send)
	{
		int num_this_time = bytes_to_send > 4096 ? 4096 : bytes_to_send;
		ret = sendfile(sess->data_fd, fd, NULL, num_this_time);
		if (ret == -1)
		{
			flag = 2;
			break;
		}
		limit_rate(sess, ret, 0);
		if (sess->abor_received)
		{
			flag = 2;
			break;
		}

		bytes_to_send -= ret;
	}

	if (bytes_to_send == 0)
	{
		flag = 0;
	}

	//�ر������׽���
	close(sess->data_fd);
	sess->data_fd = -1;

	close(fd);
	if (flag == 0 && !sess->abor_received)
	{
		//226
		ftp_reply(sess, FTP_TRANSFEROK, "Transfer complete.");
	}
	else if (flag == 2)
	{
		//426
		ftp_reply(sess, FTP_BADSENDNET, "Failure writing to network stream.");
		return;
	}
	check_abor(sess);

	//���¿�����������ͨ������
	start_cmdio_alarm();
}

static void do_stor(session_t *sess)
{
	upload_common(sess, 0);
}

static void do_appe(session_t *sess)
{
	upload_common(sess, 1);
}

static void do_list(session_t *sess)
{
	//������������
	if (get_transfer_fd(sess) == 0)
	{
		return;
	}
	//150
	ftp_reply(sess, FTP_DATACONN, "Here comes the directory listing.");

	//�����б�
	list_common(sess, 1);

	//�ر������׽���
	close(sess->data_fd);
	sess->data_fd = -1;
	//226
	ftp_reply(sess, FTP_TRANSFEROK, "Directory send OK.");

}

static void do_nlst(session_t *sess)
{
	if (get_transfer_fd(sess) == 0)
	{
		return;
	}
	//150
	ftp_reply(sess, FTP_DATACONN, "Here comes the directory listing.");

	//�����б�
	list_common(sess, 0);

	//�ر������׽���
	close(sess->data_fd);
	sess->data_fd = -1;
	//226
	ftp_reply(sess, FTP_TRANSFEROK, "Directory send OK.");
}

static void do_rest(session_t *sess)
{
	sess->restart_pos = str_to_longlong(sess->arg);
	char text[1024] = {0};
	sprintf(text, "Restart position accepted (%lld)", sess->restart_pos);
	ftp_reply(sess, FTP_RESTOK, text);
}

static void do_abor(session_t *sess)
{
	ftp_reply(sess, FTP_ABOR_NOCONN, "No transfer to ABOR.");
}

static void do_pwd(session_t *sess)
{
	char text[1024] = {0};
	char dir[1024+1] = {0};
	getcwd(dir, 1204);

	sprintf(text, "\"%s\"", dir);
	ftp_reply(sess, FTP_PWDOK, text);
}

static void do_mkd(session_t *sess)
{
	//0777 & umask
	if (mkdir(sess->arg, 0777) < 0)
	{
		ftp_reply(sess, FTP_FILEFAIL, "Create directory operation failed.");
		return;
	}
	char text[1024] = {0};
	if (sess->arg[0] == '/')
	{
		sprintf(text, "%s created", sess->arg);
	}
	else
	{
		char dir[1024+1] = {0};
		getcwd(dir, 1024);
		if (dir[strlen(dir)-1] == '/')
		{
			sprintf(text, "%s%s created", dir, sess->arg);
		}
		else
		{
			sprintf(text, "%s/%s created", dir, sess->arg);
		}
	}
	
	ftp_reply(sess, FTP_MKDIROK, text);

}

static void do_rmd(session_t *sess)
{
	if (rmdir(sess->arg) < 0)
	{
		ftp_reply(sess, FTP_FILEFAIL, "Remove directory operation failed.");
	}
	
	ftp_reply(sess, FTP_RMDIROK, "Remove directory operation successful.");
}

static void do_dele(session_t *sess)
{
	if (unlink(sess->arg) < 0)
	{
		ftp_reply(sess, FTP_FILEFAIL, "Delete operation failed.");
		return;
	}

	ftp_reply(sess, FTP_DELEOK, "Delete operation successful.");
}

static void do_rnfr(session_t *sess)
{
	//350 Ready for RNTO.
	sess->rnfr_name = (char *)malloc(strlen(sess->arg) + 1);
	memset(sess->rnfr_name, 0, strlen(sess->arg) + 1);
	strcpy(sess->rnfr_name, sess->arg);
	ftp_reply(sess, FTP_RNFROK, "Ready for RNTO.");
}

static void do_rnto(session_t *sess)
{
	if (sess->rnfr_name == NULL)
	{
		//503 RNFR required first.
		ftp_reply(sess, FTP_NEEDRNFR, "RNFR required first.");
		return;
	}
	
	rename(sess->rnfr_name, sess->arg);
	ftp_reply(sess, FTP_RENAMEOK, "Rename successful.");

	free(sess->rnfr_name);
	sess->rnfr_name = NULL;
}

static void do_site(session_t *sess)
{
	//SITE CHMOD <perm> <file>
	//SITE UMASK [umask]
	//SITE HELP

	char cmd[100] = {0};
	char arg[100] = {0};

	str_split(sess->arg, cmd, arg, ' ');
	str_upper(cmd);

	if (strcmp(cmd, "CHMOD") == 0)
	{
		do_site_chmod(sess, arg);
	}
	else if (strcmp(cmd, "UMASK") == 0)
	{
		do_site_umask(sess, arg);
	}
	else if (strcmp(cmd, "HELP") == 0)
	{
		ftp_reply(sess, FTP_SITEHELP, "CHMOD UMASK HELP");
	}
	else
	{
		ftp_reply(sess, FTP_BADCMD, "Unknown SITE command.");
	}

}

static void do_syst(session_t *sess)
{
	ftp_reply(sess, FTP_SYSTOK, "UNIX Type: L8");
}

static void do_feat(session_t *sess)
{
	ftp_lreply(sess, FTP_FEAT,"Features:");
	writen(sess->ctrl_fd, " EPRT\r\n", strlen(" EPRE\r\n"));
	writen(sess->ctrl_fd, " EPSV\r\n", strlen(" EPSV\r\n"));
	writen(sess->ctrl_fd, " MDTM\r\n", strlen(" MDTM\r\n"));
	writen(sess->ctrl_fd, " PASV\r\n", strlen(" PASV\r\n"));
	writen(sess->ctrl_fd, " REST STREAM\r\n", strlen(" REST STREAM\r\n"));
	writen(sess->ctrl_fd, " SIZE\r\n", strlen(" SIZE\r\n"));
	writen(sess->ctrl_fd, " TVFS\r\n", strlen(" TVFS\r\n"));
	writen(sess->ctrl_fd, " UTF8\r\n", strlen(" UTF8\r\n"));
	ftp_reply(sess, FTP_FEAT, "End");
}

static void do_size(session_t *sess)
{
	//550 Could not get file size.
	struct stat buf;
	if (stat(sess->arg, &buf))
	{
		ftp_reply(sess, FTP_FILEFAIL, "SIZE operation failed.");
		return;
	}

	if (!S_ISREG(buf.st_mode))
	{
		ftp_reply(sess, FTP_FILEFAIL, "Could not get file size.");
		return;
	}
	
	char text[1024] = {0};
	sprintf(text, "%lld", (long long)buf.st_size);
	ftp_reply(sess, FTP_SIZEOK, text);
}

static void do_stat(session_t *sess)
{
	ftp_lreply(sess, FTP_STATOK, "FTP server status:");
	if (sess->bw_upload_rate_max == 0)
	{
		char text[1024] = {0};
		sprintf(text, "		No session upload bandwidth limit\r\n");
		writen(sess->ctrl_fd, text, strlen(text));
	}
	else if (sess->bw_upload_rate_max > 0)
	{
		char text[1024] = {0};
		sprintf(text, "		Session upload bandwidth limit in byte/s is %u\r\n", sess->bw_upload_rate_max);
		writen(sess->ctrl_fd, text, strlen(text));
	}
	if (sess->bw_download_rate_max == 0)
	{
		char text[1024] = {0};
		sprintf(text, "		No session download bandwidth limit\r\n");
		writen(sess->ctrl_fd, text, strlen(text));
	}
	else if (sess->bw_download_rate_max > 0)
	{
		char text[1024] = {0};
		sprintf(text, "		Session download bandwidth limit in byte/s is %u\r\n", sess->bw_download_rate_max);
		writen(sess->ctrl_fd, text, strlen(text));
	}

	char text[1024] = {0};
	sprintf(text, "At session startup, client count was %u\r\n", sess->num_clients);
	writen(sess->ctrl_fd, text, strlen(text));

	ftp_reply(sess, FTP_STATOK, "End of status");
}


static void do_noop(session_t *sess)
{
	ftp_reply(sess, FTP_NOOPOK, "NOOP ok.");
}

static void do_help(session_t *sess)
{
	ftp_lreply(sess, FTP_HELP, "The following commands are recognized.");
	writen(sess->ctrl_fd,
		" ABOR ACCT ALLO APPE CDUP CWD  DELE EPRT EPSV FEAT HELP LIST MDTM MKD\r\n", 
		strlen(" ABOR ACCT ALLO APPE CDUP CWD  DELE EPRT EPSV FEAT HELP LIST MDTM MKD\r\n"));

	writen(sess->ctrl_fd,
		" MODE NLST NOOP OPTS PASS PASV PORT PWD  QUIT REIN REST RETR RMD  RNFR\r\n", 
		strlen(" MODE NLST NOOP OPTS PASS PASV PORT PWD  QUIT REIN REST RETR RMD  RNFR\r\n"));

	writen(sess->ctrl_fd,
		" RNTO SITE SIZE SMNT STAT STOR STOU STRU SYST TYPE USER XCUP XCWD XMKD\r\n", 
		strlen(" RNTO SITE SIZE SMNT STAT STOR STOU STRU SYST TYPE USER XCUP XCWD XMKD\r\n"));

	writen(sess->ctrl_fd, " XPWD XRMD\r\n", strlen(" XPWD XRMD\r\n"));

	ftp_reply(sess, FTP_HELP, "Help OK.");
}

static void do_site_chmod(session_t *sess, char *chmod_arg)
{
	//SITE CHMOD <perm> <file>
	if (strlen(chmod_arg) == 0)
	{
		ftp_reply(sess, FTP_BADCMD, "SITE CHMOD needs 2 arguments.");
		return;
	}

	char perm[100] = {0};
	char file[100] = {0};
	str_split(chmod_arg, perm, file, ' ');
	if (strlen(file) == 0)
	{
		ftp_reply(sess, FTP_BADCMD, "SITE CHMOD needs 2 arguments.");
		return;
	}

	unsigned int mode = str_octal_to_uint(perm);
	if (chmod(file, mode) < 0)
	{
		ftp_reply(sess, FTP_CHMODOK, "SITE CHMOD command failed.");
	}
	else
	{
		ftp_reply(sess, FTP_CHMODOK, "SITE CHMOD command ok.");
	}

}
static void do_site_umask(session_t *sess, char *umask_arg)
{
	//SITE UMASK [umask]
	if (strlen(umask_arg) == 0)
	{
		char text[1024] = {0};
		sprintf(text, "Your current UMASK is 0%o", tunable_local_umask);
		ftp_reply(sess, FTP_UMASKOK, text);
	}
	else
	{
		unsigned int um = str_octal_to_uint(umask_arg);
		umask(um);
		tunable_local_umask = um;
		char text[1024] = {0};
		sprintf(text, "UMASK set to 0%o", um);
		ftp_reply(sess, FTP_UMASKOK, text);
	}
}