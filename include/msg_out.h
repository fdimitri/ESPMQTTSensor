extern void msg_to_system(unsigned int msg_id, ...);
extern void msg_to_serial(unsigned int msg_id, ...);

extern struct msgSystemList *get_system_message_by_id(unsigned int id);
