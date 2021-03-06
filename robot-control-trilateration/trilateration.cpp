#include "stdafx.h"
#include <iostream>
#include <Winsock.h>
#include <stdio.h>
#include <ctime>
#include <stdlib.h>
#include <conio.h>
#include <cmath>
#include <vector>
#include <fstream>
#include "RobotTr.h"
#pragma comment(lib, "ws2_32.lib")
#define M_PI 3.1416
#define _CRT_SECURE_NO_WARNINGS

#define SERVICE_PORT 10007
using namespace std;

bool bTerminate = false;
// объявления типа матрицы
//typedef std::vector< std::vector< std::vector<int> > > matX;

int main()
{
	RobotTr robot;
	SOCKET s;
	sockaddr_in serv_addr;
	WSADATA wsadata;
	char sName[128];
	WSAStartup(MAKEWORD(2, 2), &wsadata);
	// ��������� ����� ������� ������
	gethostname(sName, sizeof(sName));
	printf("\nClient host: %s\n", sName);
	int tcp_or_udp = 1;
	std::cout << "Enter 0 for UDP or 1 for TCP" << std::endl;
	std::cin >> tcp_or_udp;
	if (tcp_or_udp > 0) {
		if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
			fprintf(stderr, "Can't create socket\n");
			exit(1);
		}
	}
	else {
		if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
			fprintf(stderr, "Can't create socket\n");
			exit(1);
		}
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	// ��������� ������ �� ���� ��������� ������� �����������, � ��������� localhost
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	// �������������� ������� ����
	serv_addr.sin_port = htons((u_short)SERVICE_PORT);
	// �����������
	while (connect(s, (sockaddr*)&serv_addr, sizeof(serv_addr)) == INVALID_SOCKET) {
		fprintf(stderr, "Can't connect\n");
	}
	send(s, "Begin", strlen("Begin"), 0);
	char sReceiveBuffer[1024] = { 0 };
	robot.receiveMess(s);
	ofstream dots;
	dots.open("dots.txt");
	float* speed;
	float sp_1 = 25;
	float sp_2 = 25;
	int ex = 0;
	float dist = 0, past_dist = 0, past_delta = 0, delta_dist = 0;
	int X = 0, Y = 0;
	while (!robot.bTerminate) {
		char sReceiveBuffer[1024] = { 0 };
		//_getch();
		if (robot.target_three > 0) {
			std::cout << "cake is a lie" << std::endl;
			_getch();
			closesocket(s);
			WSACleanup();
			dots.close();
		} else if (robot.target_two > 0) {
			std::cout << "Alright" << std::endl;
			float aero_dist = sqrt((96100. / robot.ex_robot) - (96100. / robot.base));
			speed[0] = 0;
			speed[1] = 0;
			ex = aero_dist;
		} else if (robot.target_one > 0) {
			//_getch();
			std::cout << "Okay. Let's go!" << std::endl;
			//_getch();
			if (robot.beakon == 0.) robot.beakon = 1.;
			past_dist = dist;
			past_delta = delta_dist;
			dist = 310. / robot.beakon;
			delta_dist = dist - past_dist;
			std::cout << "Current speed: " << sp_1 << ", " << sp_2 << std::endl;
			std::cout << "Distation to beakon: " << dist << std::endl;
			std::cout << "Past distation to beakon: " << past_dist << std::endl;
			std::cout << "Delta: " << delta_dist << std::endl;
			if ((delta_dist - past_delta) < 0) {
				speed[0] = 100;
				speed[1] = 100;
			} else {
				speed[0] = 100;
				speed[1] = 5;
			}
			if (speed[0] > 100) speed[0] = 100;
			if (speed[0] < -100) speed[0] = -100;
			if (speed[1] > 100) speed[1] = 100;
			if (speed[1] < -100) speed[1] = -100;
		} else {
			std::cout << "-------------------------" << std::endl;
			std::cout << "TARGET X: " << X << std::endl;
			std::cout << "TARGET Y: " << Y << std::endl;
			speed = robot.regulator(X, Y);
			
		}
		robot.sendMess(s, speed[0], speed[1], ex);
		robot.receiveMess(s);
		dots << robot.x << " " << robot.y << std::endl;
		if (robot.dead == 1) break;
	}
	closesocket(s);
	WSACleanup();
	dots.close();
	_getch();
	return 0;
}