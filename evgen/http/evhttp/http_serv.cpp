#include <sys/queue.h>
#include <stdio.h>
#include <event.h>
#include <evhttp.h>
#include <evdns.h>
#include <stdlib.h>
#include <unistd.h>
#include <map>
#include <string>
#include <string.h>
#include <pthread.h>
#include <set>
#include <fcntl.h>
#include <algorithm>
#include <vector>
#include <errno.h>
#include <sys/stat.h>

using namespace std;

//обработка события 
//	request - структура запроса
//	par 	- параметры
static void http_img_cb(struct evhttp_request *request,void *par)
{
	struct evbuffer *evb;
	const char *fname;	
	struct evkeyvalq uri_params;

//создаем буфер ввода/вывода
	evb=evbuffer_new();
	printf("Request from:%s:%d URL:%s\n",request->remote_host,request->remote_port,request->uri);

//парсим строку запроса
	evhttp_parse_query(request->uri,&uri_params);
//ищем в заголовке параметр name->??? который указывает какой файл запрашивается
	fname=evhttp_find_header(&uri_params,"name");
//иначе неверный запрос
	int fd;
	if (!fname)
	{
		//записываем в буффер то что будет отображено клиенту
		evbuffer_add_printf(evb,"Bad request");
		//отслаем запрос
		evhttp_send_reply(request,HTTP_BADREQUEST,"Bad request",evb);
		evhttp_clear_headers(&uri_params);
		evbuffer_free(evb);
		return;
	}
	
	//fstat - системная ин-фа о файле
	if (fd=open(fname,O_RDONLY)<0)
	{
		evbuffer_add_printf(evb,"File %s not found",fname);
		evhttp_send_reply(request,HTTP_NOTFOUND,"File not found",evb);
		evhttp_clear_headers(&uri_params);
		evbuffer_free(evb);
		close(fd);
		return;
	}

	struct stat stbuf;
	if (fstat(fd,&stbuf)<0)
	{
		evbuffer_add_printf(evb,"File %s not found",fname);
		evhttp_send_reply(request,HTTP_NOTFOUND,"File not found",evb);
		evhttp_clear_headers(&uri_params);
		evbuffer_free(evb);
		close(fd);
		return;
	}

	int total_read_bytes=0;
	int read_bytes;
	int sizefile=stbuf.st_size;
	while(total_read_bytes < sizefile)
	{
		read_bytes=evbuffer_read(evb,fd,sizefile);
		printf("!%d\n",read_bytes);
		if (read_bytes < 0)
		{
			evbuffer_add_printf(evb,"Error reading file %s",fname);
			evhttp_send_reply(request,HTTP_NOTFOUND,"File not found",evb);
			evhttp_clear_headers(&uri_params);
			evbuffer_free(evb);
			close(fd);
			return;
		}
		total_read_bytes+=read_bytes;
	}

	
	evhttp_add_header(request->output_headers,"Content-Type","image/jpeg");

	char sz[90];
	sprintf(sz,"%d",sizefile);
	printf("|%s|\n",sz);
//	evhttp_add_header(request->output_headers,"Content-Length",sz);
	evhttp_send_reply(request,HTTP_OK,"HTTP_OK",evb);

	evhttp_clear_headers(&uri_params);
	evbuffer_free(evb);
	close(fd);

	return;
}

//необходимо ввести адрес и порт
int main(int argc,char *argv[])
{
//объявляем основные переменные 
	struct event_base *ev_base;
	struct evhttp *ev_http;
//проверяем ввели ли ip и порт
	if (argc!=3)
	{
		printf("Bad argument\n");
		exit(1);
	}
//инициализируем библиотеку
	ev_base=event_init();
//сощдаем http сервер
	ev_http=evhttp_new(ev_base);
//делаем прослушку по введенному адресу и порту
	if (evhttp_bind_socket(ev_http,argv[1],(u_short)atoi(argv[2])))
	{
		printf("Error with bind\n");
		exit(1);
	}
//назначаем callback обработчик http_img_cb с параметрами NULL на сервак
	evhttp_set_cb(ev_http,"/img",http_img_cb,NULL);
//бесконечный цикл обработки событий
	event_base_dispatch(ev_base);

	return 0;
}
