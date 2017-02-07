#include <linux/kernel.h> //strip IP dst/src
#include <linux/module.h>
#include <linux/init.h>
#include <linux/in.h>
#include <net/sock.h>
#include <linux/ip.h>
#include <linux/ioctl.h>
#include <linux/skbuff.h>
#include <linux/inet.h>
#include <linux/random.h>
#include <linux/inetdevice.h>
#include <linux/netdevice.h>

#define SENDPORT 9999


#define A_CHANGE 0x43
#define A_UPDATE 0x55
#define T_WLAN   0x57
#define T_ETH	 0x45
#define T_VIRT	 0x56
#define SPEC  	 0x01
#define RAND	 0x00



#define RECV_PORT 1215
#define MAX_BUFF  100
#define COM_BUFF  12
#define MAX_ATP	  5
#define IFMAX 	  7
#define IDSIZE 	  3

static DECLARE_COMPLETION(threadcomplete);
static struct socket *recv_cc_socket=NULL;
static struct socket *send_cc_socket=NULL;
//keeps count of number of interfaces
//represents the index of the next free if slot in struct
unsigned short if_count; 
unsigned short cc_init;
unsigned char CCid1[] = {0xB7, 0xE2, 0xAC};
/* All info derived from the udp message*/
struct command_center_info{
	__be32 ipv4address;
	unsigned short cc_port; //port CC listen
	char ccid[IDSIZE];
};
struct net_interface_info{
	__be32 ipv4address;
	unsigned char ip_as_str[16];
	struct net_device *dev;
	char name[IFNAMSIZ]; //IFNAMSIZ linux kernel constant
};
struct workq_data{
    struct work_struct worker;
	struct sock *sk;
};
struct net_interface_info net_if[IFMAX];
struct workq_data wq_data;
struct workqueue_struct *wq;
struct command_center_info cc_center;

static void callback_data(struct sock *sk, int bytes){
	wq_data.sk = sk;
	queue_work(wq, &wq_data.worker);
}
int verify_CC_id(char *mesg){
	printk(KERN_DEBUG "mesg %s", mesg);
	if(!memcmp(mesg, CCid1, 3)){
		return 1;
	}
	return 2;
}
unsigned char gen_rand_key(void){
	unsigned char k;
	get_random_bytes(&k, sizeof(k));
	//printk(KERN_DEBUG "Random value %01x", k);
	return k;
}
void update_cc_info(unsigned short port, __be32 addr){
	cc_center.cc_port = port;
	cc_center.ipv4address = addr;
	cc_center.ccid[0]= 0xB7;
	cc_center.ccid[1]= 0xE2;
	cc_center.ccid[2]= 0xAC;
	return;
}
int find_index(char name[], int size){//IFNAMSIZ linux kernel constant)
	int i;
	printk(KERN_DEBUG "Searching for %s\n", name);
	for (i = 0; i < if_count; i++){
		printk(KERN_DEBUG "Comparing %s to %s \n", name, net_if[i].name);
		if(memcmp(name, net_if[i].name, size) == 0){
			printk(KERN_DEBUG "FOUND!!!!\n");
			return i;
		}
	}
	return -1;
} 
/*
 *	Convert an ASCII string to binary IP. 
 */
__u32 in_aton(const char *str)
{
	unsigned long l;
	unsigned int val;
	int i;
	l = 0;
	for (i = 0; i < 4; i++) 
	{
		l <<= 8;
		if (*str != '\0') 
		{
			val = 0;
			while (*str != '\0' && *str != '.') 
			{
				val *= 10;
				val += *str - '0';
				str++;
			}
			l |= val;
			if (*str != '\0') 
				str++;
		}
	}
	return(htonl(l));
}
void set_in_ntoa_struct(int struct_index){
	char *p;
	__u32 temp;
	temp = be32_to_cpu(net_if[struct_index].ipv4address);
	p = (char *) &temp;
	printk("struct:: %d.%d.%d.%d",(p[3] & 255), (p[2] & 255), (p[1] & 255), (p[0] & 255));
	sprintf(net_if[struct_index].ip_as_str, "%d.%d.%d.%d",(p[3] & 255), (p[2] & 255), (p[1] & 255), (p[0] & 255));
	return;
}
char *in_ntoa(__u32 in){
	static char buff[18];
	char *p;
	memset(buff, 0, sizeof(buff));

	p = (char *) &in;
	printk("WTF:: %d.%d.%d.%d",(p[3] & 255), (p[2] & 255), (p[1] & 255), (p[0] & 255));
	sprintf(buff, "%d.%d.%d.%d",(p[3] & 255), (p[2] & 255), (p[1] & 255), (p[0] & 255));
	return(buff);
}


void cc_send(char *mesg, __be32 ipaddr, unsigned short sendPort, unsigned short mesglen) {
		//unsigned short *port;
		int len;
		struct msghdr msg;
		struct iovec iov;
		mm_segment_t oldfs;
		struct sockaddr_in to;

		//generate answer message 
		memset(&to,0, sizeof(to));
		to.sin_family = AF_INET;
		to.sin_addr.s_addr = ipaddr;  //******
		printk(KERN_DEBUG "SEND TO IP: %pI4\n" , &to.sin_addr.s_addr);
		//port = sendPort; // ******
		to.sin_port = htons(sendPort);
		printk(KERN_DEBUG "SEND TO Port: %d\n", ntohs(to.sin_port));
		memset(&msg,0,sizeof(msg));
		msg.msg_name = &to;
		msg.msg_namelen = sizeof(to);
		// send the message back 
		printk(KERN_DEBUG "SEND mesg: %s\n", mesg);
		iov.iov_base = mesg;//skb->data+8;
		iov.iov_len  = mesglen; //*****
		msg.msg_control = NULL;
		msg.msg_controllen = 0;
		msg.msg_iov = &iov;
		msg.msg_iovlen = 1;
		// adjust memory boundaries 	
		oldfs = get_fs();
		set_fs(KERNEL_DS);
		len = sock_sendmsg(send_cc_socket, &msg, mesglen); //****
		printk(KERN_DEBUG "len from sock: %d\n", len);
		set_fs(oldfs);
		// free the initial skb 
		
}

void mtd_targeted(int index, __u32 key){
	unsigned short cur_if;
	struct in_device *in_dev;
  	struct in_ifaddr *if_info;
	struct net_device *dev;
	//struct net_device_ops ops;
	__be32 n_key;
 	__u32 test2;
 	test2 = 0x00FFFFFF;
 	//test2 = test2 | key;
 	n_key = cpu_to_be32(key);
	cur_if = 0;
	dev = first_net_device(&init_net);
	while (dev && cur_if < IFMAX) {
    	in_dev = (struct in_device *)dev->ip_ptr;
		for (if_info = in_dev->ifa_list; if_info != NULL; if_info=if_info->ifa_next){
				if (cur_if == index){
					//dev->netdev_ops->ndo_stop(dev);
					if_info->ifa_address = if_info->ifa_address & test2;
					if_info->ifa_local = if_info->ifa_local & test2;
					if_info->ifa_address = if_info->ifa_address | n_key;
					if_info->ifa_local = if_info->ifa_local | n_key;
					printk(KERN_DEBUG "New Test::: %pI4", &if_info->ifa_local); //I FOUND IT!!!!!!!
					//dev->netdev_ops->ndo_open(dev);
				}	
		}
		cur_if++;
    	dev = next_net_device(dev);
	}
	return;
}
void build_update_targeted(int index, unsigned char *ret_code){
	unsigned char payload[200];
	char r[1];
	r[0] = ret_code[0];
	printk(KERN_DEBUG "Building Targeted Update\n");
	memset(payload, 0, sizeof(payload));
	strncpy(payload, CCid1, 3);
	strncat(payload, r, sizeof(r));
	strcat(payload,"|");
	strncat(payload, net_if[index].name, sizeof(net_if[index].name));
	strcat(payload,"|");
	if(net_if[index].ip_as_str[0] == '\0'){
		strcat(payload, "NO CONNECTION");
	}else{
		strncat(payload, net_if[index].ip_as_str, 16);
	}
	printk(KERN_DEBUG "***Targeted Update Complete***\n");
	cc_send(payload, cc_center.ipv4address, SENDPORT, sizeof(payload));
	memset(payload, 0, sizeof(payload));
	return;
}

void build_update_sys(void){
	unsigned char payload[200];
	char u[1] = {0x55};
	int i;
	memset(payload, 0, sizeof(payload));
	strncpy(payload, CCid1, 3);
	strncat(payload, u, sizeof(u));
	for(i = 0; i < if_count; i++){
		strcat(payload,"|");
		strncat(payload,net_if[i].name, sizeof(net_if[0].name));
		strcat(payload,"|");
		if(net_if[i].ip_as_str[0] == '\0'){
			strcat(payload, "NO CONNECTION");
		}else{
			strncat(payload, net_if[i].ip_as_str,16);
		}
		printk(KERN_DEBUG "Building Update iteration #%d\n",i+1);
	}
	printk(KERN_DEBUG "\n\n***Update build complete***\n\n");
	cc_send(payload, cc_center.ipv4address, SENDPORT, sizeof(payload));
	memset(payload, 0, sizeof(payload));
	return;
}

/*
 * Function is used purely to know exact sizes for memory optimization
 * Add all unknown sizes for proper medeclaration order
 */
void print_sizes(void){
	printk(KERN_DEBUG "Size of struct net_device: %lu \n", sizeof(struct net_device*));
	printk(KERN_DEBUG "Size of struct work_struct : %lu \n", sizeof(struct work_struct));
	printk(KERN_DEBUG "Size of struct sock: %lu \n", sizeof(struct sock*));
	printk(KERN_DEBUG "Size of __be32: %lu \n", sizeof(__be32));
	printk(KERN_DEBUG "Size of __u32: %lu \n", sizeof(__u32));
	printk(KERN_DEBUG "Size of name[IFNAMSIZ]: %lu \n", sizeof(net_if[0].name));
	/*Begin MTD specific structs */
	printk(KERN_DEBUG "Size of struct command_center_info: %lu \n", sizeof(struct command_center_info));
	printk(KERN_DEBUG "Size of struct net_interface_info: %lu \n", sizeof(struct net_interface_info));
	printk(KERN_DEBUG "Size of struct workq_data: %lu \n", sizeof(struct workq_data));
	return;
}

void retrieve_ifdata(void){
	unsigned short cur_if;
	unsigned short pcntr;
	struct in_device *in_dev;
  	struct in_ifaddr *if_info;
	struct net_device *dev;
 	//KEEP FOR REFERENCE
 	//__be32 test;
 	//__u32 test2;
 	//test2 = 0x00FF;
 	//test = cpu_to_be32(test2);
	cur_if = 0;
	dev = first_net_device(&init_net);
	while (dev && cur_if < IFMAX) {
    	net_if[cur_if].dev = dev;
    	strncpy(net_if[cur_if].name, dev->name, sizeof(dev->name));
    	in_dev = (struct in_device *)dev->ip_ptr;
		for (if_info = in_dev->ifa_list; if_info != NULL; if_info=if_info->ifa_next){
				net_if[cur_if].ipv4address = if_info->ifa_address;
				set_in_ntoa_struct(cur_if);
				if (cur_if == 1)
				{
					//ifa_local - local address for interface
					//ifa_address - actual destination address ????????

					//if_info->ifa_local = if_info->ifa_local | test;
					//printk(KERN_DEBUG "Test::: %pI4", &if_info->ifa_local); //I FOUND IT!!!!!!!
				}
				
		}
		cur_if++;
    	dev = next_net_device(dev);
	}
	 //update the iterface counter
	if_count = cur_if;
	//Strictly for debugging, Remove when not needed	
	for(pcntr = 0; pcntr < cur_if; pcntr++){
		printk("Network Interface Name: %s\n", net_if[pcntr].dev->name);
		printk("Network Interface ip: %pI4\n", &net_if[pcntr].ipv4address);
		printk("Network Interface ip as string: %s\n\n", net_if[pcntr].ip_as_str);
	}
	return;
}

/*
 * @cmd_lookup - used to lookup received command from CC center
 * NOT IMPLEMENTED
 */
void cmd_lookup(char *cmd, char *mesg){
	unsigned char err_mesg[MAX_BUFF];
	char if_name[IFNAMSIZ];
	int temp;
	unsigned char temp_c[1];
	memset(err_mesg, 0, sizeof(err_mesg));
	printk("CMD message len: %i message: %s\n", (int)strlen(cmd), cmd); /*8 for udp header*/	
	printk(KERN_DEBUG "memcmp: %d\n", memcmp(cmd, cc_center.ccid, 1));
	switch((cmd[3] & 0xFF)){
		case 0x55:
			printk(KERN_DEBUG "Command found -> Update ALL System Interfaces\nCode: 0x55\n");
			build_update_sys();
			break;
		case 0x56:
			printk(KERN_DEBUG "Command found -> Perform MTD on VIRTUAL\nCode: 0x56\n");
			strcpy(if_name, "wlan0");
			temp = find_index(if_name, 4);
			temp_c[0] = gen_rand_key();
			if(temp<0){break;} //NEED TO DEVELOP ERROR FUNCTIONS
			mtd_targeted(temp, temp_c[0]);
			retrieve_ifdata();
			temp_c[0] = 0x56;
			build_update_targeted(temp, temp_c);	
			break;
		case 0x57:
			printk(KERN_DEBUG "Command found -> Perform MTD on WLAN\nCode: 0x57\n");
			strcpy(if_name, "wlan0");
			temp = find_index(if_name, 4);
			temp_c[0] = 0x57;
			if(temp<0){break;} 		
			mtd_targeted(temp, 0x0044);
			retrieve_ifdata();
			build_update_targeted(temp, temp_c);
			break;
		case 0x45:
			printk(KERN_DEBUG "Command found -> Perform MTD on ETHERNET\nCode: 0x45\n");
			strcpy(if_name, "eth0");
			temp = find_index(if_name, 4);
			temp_c[0] = 0x45;
			if(temp<0){break;} 		
			mtd_targeted(temp, 0x0044);
			retrieve_ifdata();
			build_update_targeted(temp, temp_c);
			break;
		default:
			temp_c[0] = 0xFF;
			strncpy(err_mesg, CCid1, 3);
			strncat(err_mesg, temp_c, 1);
			strcat(err_mesg,"Command code not found");
			cc_send(err_mesg, cc_center.ipv4address, SENDPORT, strlen(err_mesg));
			break;
	}
	return;
}

void cc_recv(struct work_struct *data){
	struct  workq_data *thread = container_of(data, struct  workq_data, worker);
	int len = 0;
	//unsigned short dport = 0;
	printk("MTD MODULE: Listener activated...\n");
	/* as long as there are messages in the receive queue of this socket*/
	while((len = skb_queue_len(&thread->sk->sk_receive_queue)) > 0){
		struct sk_buff *skb = NULL;
		char mesg_cc[MAX_BUFF] = {};
		char err_mesg[MAX_BUFF] = {};
		unsigned char cmd[MAX_BUFF] = {};
		struct iphdr *iph;
		//__be32 ipaddress; //NOT REQUIRED, MAY BE USEFUL LATER
		// receive packet 
		skb = skb_dequeue(&thread->sk->sk_receive_queue);
		strncpy(cmd, (skb->data+8), (skb->len - 8));
		iph = ip_hdr(skb);
		//dport = (unsigned short)thread->sk->sk_num;
		//ipaddress = iph->saddr;
		printk(KERN_DEBUG "%pI4 \n", &iph->saddr);
		printk(KERN_DEBUG "message len: %i message: %s\n", skb->len - 8, skb->data+8); /*8 for udp header*/
		//printk(KERN_DEBUG "%d \n", dport);
		if (cc_init != 1){
			cc_init = verify_CC_id(skb->data+8);
			printk(KERN_DEBUG "***Command and Control Center ID verified*** \n Return Code: %d\n", cc_init);
			if(cc_init == 2){
				strncpy(err_mesg, "Invalid Command Center ID\n", sizeof(err_mesg));
				cc_send(err_mesg, iph->saddr, ((unsigned short)thread->sk->sk_num), sizeof(err_mesg));
			}else{
				update_cc_info(((unsigned short)thread->sk->sk_num), iph->saddr);
			}
		}
		//build_update_sys();
		//printk(KERN_DEBUG "memcmp test IP: %d", memcmp(&cc_center.ipv4address, &iph->saddr, sizeof(__be32)));
		//printk(KERN_DEBUG "CC port %d  CC addr %pI4 \n", cc_center.cc_port, &cc_center.ipv4address);
		
		//retrieve_ifdata();
		cmd_lookup(cmd, mesg_cc);
		kfree_skb(skb);
		memset(cmd,0, sizeof(cmd));
	}
}
/*
 *	Initialize the inbound socket, bind to addr space, pass callback
 *	Initialize socket for sending data, 
 */
static int __init MTD_Module_Init( void )
{
	struct sockaddr_in recv_socket;
	int servererror;
	printk("MTD MODULE: Initializing\n");
	/* socket to receive data */
	/* SEE GLOBAL DATA -> recv_cc_socket */
	if (sock_create(PF_INET, SOCK_DGRAM, IPPROTO_UDP, &recv_cc_socket) < 0) {
		printk( KERN_ERR "MTD MODULE: Error creating recv_cc_socket...exiting\n" );
		return -EIO;
	}
	recv_socket.sin_family = AF_INET;
	recv_socket.sin_addr.s_addr = INADDR_ANY;
	recv_socket.sin_port = htons((unsigned short)RECV_PORT);
	servererror = recv_cc_socket->ops->bind(recv_cc_socket, (struct sockaddr *) &recv_socket, sizeof(recv_socket ));
	if (servererror) {
		sock_release(recv_cc_socket);
		return -EIO;
	}
	//passing callback data
	recv_cc_socket->sk->sk_data_ready = callback_data;

	 /* socket to send data */
	if (sock_create(PF_INET, SOCK_DGRAM, IPPROTO_UDP, &send_cc_socket) < 0) {
		printk( KERN_ERR "MTD MODULE: Error creating send_cc_socket.n" );
		return -EIO;
	}
	/* create work queue */	
	printk("MTD MODULE: Threading for receive\n");
	INIT_WORK(&wq_data.worker, cc_recv); //Essentially a special kernel thread 
	wq = create_singlethread_workqueue("recv_thread"); //Naming the thread
	if (!wq){
		return -ENOMEM;
	}
	retrieve_ifdata();
	//print_sizes(); 
	return 0;
}

static void __exit MTD_Module_Exit( void )
{
	if (recv_cc_socket)
		sock_release(recv_cc_socket);
	if (send_cc_socket)
		sock_release(send_cc_socket);
	if (wq) {
        flush_workqueue(wq);
        destroy_workqueue(wq);
	}
	printk("MTD MODULE EXIT COMPLETE\n");
}

module_init(MTD_Module_Init);
module_exit(MTD_Module_Exit);
MODULE_LICENSE("GPL");
