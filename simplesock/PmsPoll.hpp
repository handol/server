/*
참고 자료
AIX poll() - http://publib.boulder.ibm.com/infocenter/pseries/v5r3/index.jsp?topic=/com.ibm.aix.basetechref/doc/basetrf1/poll.htm

*/


inline SOCKET PmsPoll::get_sock(unsigned int index)
{
	//if (index >= numsocks) return 0;
	return (pollset[index].fd);
}


inline unsigned int PmsPoll::event(unsigned int index)
{
	//if (index >= numsocks) return 0;	
	return (pollset[index].revents);		
}


inline bool PmsPoll::has_r_evt(unsigned int index)
{
	//if (index >= numsocks) return 0;	
	return IS_POLL_R(pollset[index].revents);	
}

inline bool PmsPoll::has_w_evt(unsigned int index)
{
	//if (index >= numsocks) return 0;	
	return IS_POLL_W(pollset[index].revents);	
}

inline bool PmsPoll::has_e_evt(unsigned int index)
{
	//if (index >= numsocks) return 0;	
	return IS_POLL_E(pollset[index].revents);	
}
	
