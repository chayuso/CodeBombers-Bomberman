#include <queue>
#include <chrono>

const int DELAY = 500;		// Milliseconds of delay

long long currentTime()
{
	return std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
}

struct info
{
    public:
        long long timeToProcess;
        int clientID;
		bool isChat;
		bool isStats;
        std::string msg;

        info(int _clientID, std::string _msg, bool chat = false, bool stats = false):
                clientID(_clientID), msg(_msg), isChat(chat), isStats(stats)
        {
			//if (clientID == 1) {//used to test 1 client with highlatency
			//	timeToProcess = currentTime() +rand() % DELAY; //Random from 1 to Delay Time
			//}
			//else{
			timeToProcess = currentTime();// +rand() % DELAY;//<---Used to create artificial latency
			//}
        }
};


template<typename T>
struct greaterThanByTime {
	bool operator()(const T &lhs, const T &rhs){
		return lhs.timeToProcess > rhs.timeToProcess;
	}
};


std::priority_queue<info, vector<info>, greaterThanByTime<info>> lat_msg;
std::priority_queue<info, vector<info>, greaterThanByTime<info>> lat_event;

