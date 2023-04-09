#include <iostream>
#include <thread>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <experimental/filesystem>
#include <unordered_set>
#include <string>
#include <fstream>
#include <regex>
#include <ctime>
#include <queue>
#include <sstream>

using namespace std;

queue <experimental::filesystem::path> storage;
int waiting = 0; //число ждущих потоков
mutex storageMutex;
condition_variable storageMutexCondVar;
condition_variable endCondVar;
unordered_set <string> used;
regex reg("([:.\/\\w]+)", regex::optimize);
thread_local queue <experimental::filesystem::path> threadStorage;

void handlerBody(int numThreads, int threadStorageSize) {
	unique_lock<mutex> lockStorage(storageMutex);
	for (;;) {
		bool stoped = false;
		if (threadStorage.empty() && storage.empty()) {
			waiting++;
			stoped = true;
		}
		if (waiting == numThreads) {
			endCondVar.notify_one();
		}
		storageMutexCondVar.wait(lockStorage, [] {return !threadStorage.empty() || !storage.empty(); });
		if (stoped) {
			waiting--;
		}
		while ((threadStorage.size() < threadStorageSize) && (!storage.empty())) {
			threadStorage.push(storage.front());
			storage.pop();
		}
		lockStorage.unlock();
		vector<experimental::filesystem::path> href;
		bool locked = false;
		while (!locked && !threadStorage.empty()) {
			experimental::filesystem::path pth = threadStorage.front();
			threadStorage.pop();
			experimental::filesystem::copy(pth, "pages/" + pth.filename().string(), experimental::filesystem::copy_options::update_existing);
			ifstream fin;
			fin.open(pth);
			string str;
			str.reserve(experimental::filesystem::file_size(pth));
			stringstream sstream(str);
			sstream << fin.rdbuf();
			str = sstream.str();
			fin.close();
			string::size_type lfind = str.find("<a href=\"file:\/\/", 0);
			while (lfind != string::npos) {
				lfind += size("<a href=\"file:\/\/")-1;
				string::size_type rfind = str.find("\">", lfind);
				string path = str.substr(lfind, rfind - lfind);
				smatch match;
				if (regex_search(path, match, reg)) {
					path = match[1].str();
					if (path.find(":") == string::npos) { //относительный путь
						experimental::filesystem::path tmp_pth1 = path, tmp_pth2 = pth.parent_path();
						href.push_back(tmp_pth2 /= tmp_pth1);
					}
					else { //абсолютный путь
						experimental::filesystem::path tmp_pth1 = path;
						href.push_back(tmp_pth1);
					}
				}
				lfind = str.find("<a href=\"", lfind);
			}
			locked = lockStorage.try_lock();
		}
		if (!locked) {
			lockStorage.lock();
			threadStorageSize++;
		}
		bool inserted = false;
		for (auto it : href) {
			if (experimental::filesystem::exists(it) && used.insert(it.string()).second) {
				storage.push(it);
				inserted = true;
			}
		}
		if (inserted) {
			storageMutexCondVar.notify_all();
		}
	}
}


int main()
{
	string start;
	getline(cin, start);
	int N, storageSize;
	cin >> N >> storageSize;
	experimental::filesystem::create_directory("pages");
	experimental::filesystem::path pth = start.substr(start.find("//") + 1);
	if (experimental::filesystem::exists(pth)) {
		storage.push(pth);
		used.insert(pth.string());
	}
	time_t time_start = clock();
	for (int i = 0; i < N; i++) {
		thread(handlerBody, N, storageSize).detach();
	}
	mutex endMutex;
	unique_lock<mutex> lockEnd(endMutex);
	endCondVar.wait(lockEnd, [N] {return waiting == N; });
	cout <<"Links: "<< used.size() << endl;
	time_t time_end = clock();
	cout <<"Time: "<< time_end - time_start <<"ms"<< endl;
}

