namespace
{
	std::list<std::string> GetLocalIPAddresses()
	{
		std::list<std::string> addresses;
		char ac[80];
		if (gethostname(ac, sizeof(ac)) == SOCKET_ERROR)
			return addresses;
		struct hostent *phe = gethostbyname(ac);
		if (phe == nullptr)
			return addresses;

		for (int i = 0; phe->h_addr_list[i] != 0; ++i)
		{
			struct in_addr addr;
			memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
			addresses.push_back(inet_ntoa(addr));
		}
		return addresses;
	}
}