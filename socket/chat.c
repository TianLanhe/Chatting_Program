#include "chat.h"

int build_packet(Packet *packet,Kind kind,...){
	va_list ap;
	packet->kind=kind;
	va_start(ap,kind);
	switch(kind){
		case enum_regist:
		case enum_modify:
		case enum_logout:
		case enum_login:packet->data=(Data)va_arg(ap,User);break;
		case enum_chat:packet->data=(Data)va_arg(ap,Message);break;
		default:return -1;
	}
	va_end(ap);
	return 0;
}
int parse_packet(Packet packet,Kind *kind,Data *data){
	*kind=packet.kind;
	*data=packet.data;
	return 0;
}

