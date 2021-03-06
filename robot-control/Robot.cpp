﻿#include "stdafx.h"
#include "Robot.h"
#include <iostream>
#include <Winsock.h>
#include <stdio.h>
#include <ctime>
#include <stdlib.h>
#include <conio.h>
#include <cmath>
#include <vector>
#include <fstream>
#include "Robot.h"
#pragma comment(lib, "ws2_32.lib")
#define M_PI 3.1416
#define _CRT_SECURE_NO_WARNINGS
#define SERVICE_PORT 10007
#pragma once
using namespace std;


Robot::Robot()
{
	std::cout << "Robot was created" << endl;
}

Robot::~Robot()
{
	std::cout << "Robot was destroyed" << endl;
	_getch();
}

char* Robot::receiveMess(SOCKET s)
{
	char sReceiveBuffer[1024] = { 0 };
	// �������� � ������������ ������ �� �������
	int nReaded = recv(s, sReceiveBuffer, sizeof(sReceiveBuffer) - 1, 0);
	// � ������ ������ (��������, ������������ �������) �������
	if (nReaded <= 0) return 0;
	// �� �������� ����� ����, ������� ����� �������������� 
	// �������� ����������� 0 ��� ASCII ������ 
	sReceiveBuffer[nReaded] = 0;
	// ����������� ������� �������� �����
	for (char* pPtr = sReceiveBuffer; *pPtr != 0; pPtr++) {
		if (*pPtr == '\n' || *pPtr == '\r') {
			*pPtr = 0;
			break;
		}
	}
	std::cout << "-------------------------\n";
	std::cout << "Receive:" << std::endl;
	std::cout << sReceiveBuffer << std::endl;
	sscanf_s(sReceiveBuffer, "0xFF 0xFF L%f R%f S%d H%f M1%d M2%d M6%d 0xEE", &l_enc, &r_enc, &sens_target, &strength, &target_one, &target_two, &dead);
	if (r_enc != 0) l_enc--;
	std::cout << "left: " << l_enc << std::endl;
	std::cout << "right: " << r_enc << std::endl;
	updatePos();
}

int Robot::sendMess(SOCKET s, int l_power, int r_power, int d_power)
{
	if (l_power > 100) l_power = 100;
	if (l_power < -100) l_power = -100;
	if (r_power > 100) r_power = 100;
	if (r_power < -100) r_power = -100;
	char control[1024] = { 0 };
	sprintf_s(control, "0xFF 0xFF L%d R%d D%d 0xEE", l_power, r_power, d_power);
	std::cout << "-------------------------\n";
	std::cout << "Send:" << std::endl;
	std::cout << control << std::endl;
	return send(s, control, strlen(control), 0);
}

float* Robot::regulator(float tar_x, float tar_y)
{
	float delta_x, delta_y, tar_ang, err_ang;
	float speed[2];
	int spd;
	delta_x = tar_x - x;
	delta_y = tar_y - y;
	tar_ang = atan2(delta_y, delta_x);
	err_ang = tar_ang - abs_ang;
	std::cout << "-------------------------" << std::endl;
	std::cout << "Current angle error: " << err_ang << std::endl;
	if (abs(err_ang) < 0.1) {
		spd = 70;
	} else {
		spd = 0;
	}
	speed[0] = round(spd - err_ang * kp);
	speed[1] = round(spd + err_ang * kp);
	return speed;
}

void Robot::track()
{
	int flag = 0;
	for (int i = border - 10; i >= -border + 10; i -= 5) {
		if (flag % 2) {
			for (int j = -border + 10; j <= border - 10; j += 20) {
				targets.push_back(-j);
				targets.push_back(-i);
			}
		} else {
			for (int j = border - 10; j >= -border + 10; j -= 20) {
				targets.push_back(-j);
				targets.push_back(-i);
			}
		}
		flag++;
	}
}


float* Robot::showXY()
{
	float coord[2];
	coord[0] = x;
	coord[1] = y;
	return coord;
}

