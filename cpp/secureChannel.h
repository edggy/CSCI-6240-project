
class secureChannel {
	public:
		virtual ssize_t send(int sockfd, const void *buf, size_t len, int flags);
		virtual ssize_t recv(int sockfd, void *buf, size_t len, int flags);
		virtual char* encrypt(char* enc, const char* msg, const char* key, unsigned long long size);
		virtual char* decrypt(char* msg, const char* enc, const char* key, unsigned long long size);
		virtual char* mac(char* mac, const char* msg, const char* key, unsigned long long size);
		virtual char* verify(bool valid, const char* msg, const char* mac, const char* key, unsigned long long size);
		virtual char* macEncrypt(char* digest, const char* msg, const char* key, unsigned long long size);
		virtual char* decryptVerify(char* msg, const char* digest, const char* key, unsigned long long size);
};
