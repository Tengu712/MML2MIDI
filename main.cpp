#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
#include <Windows.h>

#define TICK 360


// �E�񂹂�256�i����
void itoh(std::vector<UCHAR>& v, int n, int d) {
	int cnt = 0;
	int tmp_1 = n;
	std::vector<UCHAR> tmp_v;
	while(tmp_1 > 0){
		cnt = 0;
		int tmp_2 = tmp_1;
		while(tmp_2 > 256){
			tmp_2 /= 256;
			++cnt;
		}
		tmp_v.push_back((UCHAR)tmp_2);
		tmp_1 = tmp_1 % (int)pow(256, cnt);
	}
	cnt = 0;
	for(int i = 0; i < d; ++i){
		if(d-i > (int)tmp_v.size()){
			v.push_back(0x00);
		}
		else{
			v.push_back(tmp_v.at(cnt));
			++cnt;
		}
	}
}


// �ϒ���128�i����
void ttoh(std::vector<UCHAR>& v, int n){
	int tmp_1 = n;
	while(tmp_1 > 0){
		v.push_back(0x00);
		int cnt = 0;
		int tmp_2 = tmp_1;
		if(tmp_2 >= 128){
			v.back() += 0x80;
			++cnt;
		}
		while(tmp_2 >= 128){
			tmp_2 /= 128;
		}
		v.back() += (UCHAR)tmp_2;
		tmp_1 = tmp_1 % (int)pow(128, cnt);
	}
}



int main(){


    // �t�@�C���X�g���[��
    std::ifstream ifs;
    ifs.open("mml.txt");
    if (ifs.fail()) return -1;

    // �ǂݎ��
    std::string line;
    std::vector<std::string> data;
    while (getline(ifs, line)) {
        data.push_back(line);
    }

    // ����
    ifs.close();


    // ���
    int bpm = 120;
    std::vector<std::vector<UCHAR>> notes(10);
    std::vector<std::vector<UCHAR>> volumes(10);
    std::vector<std::vector<int>> waits(10);
    std::vector<int> oct = { 4,4,4,4,4,4,4,4,4,4 };
    for (int i = 0; i < (int)data.size(); ++i) {
		// �󔒍s�X�L�b�v
		if (data.at(i).size()==0) continue;
        // �e���|
        if (data.at(i).size() > 4 && data.at(i).substr(0, 4) == "bpm:") {
            bpm = std::stoi(data.at(i).substr(4));
            continue;
        }
        // �p�[�g�w��
        int part = std::stoi(data.at(i).substr(0, 1));
        // �m�[�c����
        for (int j = 2; j < (int)data.at(i).size(); ++j) {
            // ��
            if ((int)data.at(i)[j] >= 97 && (int)data.at(i)[j] <= 103) {
                // ������
                UCHAR note = 0x09;
                // ����
                if (data.at(i)[j] == 'b') note += 0x02;
                else if (data.at(i)[j] == 'c') note += 0x03;
                else if (data.at(i)[j] == 'd') note += 0x05;
                else if (data.at(i)[j] == 'e') note += 0x07;
                else if (data.at(i)[j] == 'f') note += 0x08;
                else if (data.at(i)[j] == 'g') note += 0x0A;
                // �I�N�^�[�u
                note += (UCHAR)(oct.at(part) * 12);
                // ���f
                notes.at(part).push_back(note);
				// ����
				volumes.at(part).push_back(0x7F);
            }
            // ����
            else if ((int)data.at(i)[j] >= 48 && (int)data.at(i)[j] <= 57) {
                int lng = 0;
                if (j + 1 < (int)data.at(i).size() && (int)data.at(i)[j + 1] >= 48 && (int)data.at(i)[j + 1] <= 57)
                    lng = std::stoi(data.at(i).substr(j, 2));
                else
                    lng = std::stoi(data.at(i).substr(j, 1));
                waits.at(part).push_back((4 * TICK) / lng);
            }
			// �x��
			else if (data.at(i)[j] == 'r'){
				notes.at(part).push_back(0x3C);
				volumes.at(part).push_back(0x00);
			}
            // ����
            else if (data.at(i)[j] == '+') notes.at(part).back() += 0x01;
            else if (data.at(i)[j] == '-') notes.at(part).back() -= 0x01;
            // �I�N�^�[�u
            else if (data.at(i)[j] == '>') oct.at(part)++;
            else if (data.at(i)[j] == '<') oct.at(part)--;
			// ���_
			else if (data.at(i)[j] == '.') waits.at(part).back() = waits.at(part).back() * 15 / 10;
        }

    }


	// ���t�g���b�N
	int cnt = 0;
	std::vector<std::vector<UCHAR>> tracks;
	while(notes.at(cnt).size()>0){
	    std::vector<UCHAR> track2 = {
	        0x4D,0x54,0x72,0x6B // "MTrk"
    	};
   	 	std::vector<UCHAR> tmp2_1;
    	std::vector<UCHAR> tmp2_2 = {
        	0x00,0xFF,0x21,0x01,0x00 // portA
    	};
    	for (int i = 0; i < (int)notes.at(cnt).size(); ++i) {
        	tmp2_2.push_back(0x00);
        	tmp2_2.push_back(0x90);
        	tmp2_2.push_back(notes.at(cnt).at(i));
        	tmp2_2.push_back(volumes.at(cnt).at(i));
			ttoh(tmp2_2, waits.at(cnt).at(i));
        	tmp2_2.push_back(0x80);
        	tmp2_2.push_back(notes.at(cnt).at(i));
        	tmp2_2.push_back(0x00);
    	}
    	std::vector<UCHAR> tmp2_3 = {
        	0x78,0xFF,0x2F,0x00 // �g���b�N�G���h
    	};
    	itoh(tmp2_1, (int)tmp2_2.size() + 4, 4);
    	std::copy(tmp2_1.begin(), tmp2_1.end(), std::back_inserter(track2));
    	std::copy(tmp2_2.begin(), tmp2_2.end(), std::back_inserter(track2));
    	std::copy(tmp2_3.begin(), tmp2_3.end(), std::back_inserter(track2));
		tracks.push_back(track2);
		++cnt;
	}


    // �w�b�_�[���
    std::vector<UCHAR> header = {
        0x4D,0x54,0x68,0x64, // "MThd"
        0x00,0x00,0x00,0x06, // 6 byte
        0x00,0x01, // midi�t�H�[�}�b�g 
	};
	std::vector<UCHAR> tmp0_1;
	std::vector<UCHAR> tmp0_2;
	itoh(tmp0_1, (int)tracks.size()+1, 2);
	itoh(tmp0_2, TICK, 2);
	std::copy(tmp0_1.begin(), tmp0_1.end(), std::back_inserter(header));
	std::copy(tmp0_2.begin(), tmp0_2.end(), std::back_inserter(header));


    // �g���b�N�P
    std::vector<UCHAR> track1 = {
        0x4D,0x54,0x72,0x6B, // "MTrk"
        0x00,0x00,0x00,0x13, // 19 byte
        0x00,0xFF,0x51,0x03 // BPM�O��
    };
    std::vector<UCHAR> tmp1_1;
    std::vector<UCHAR> tmp1_2 = {
        0x00,0xFF,0x58,0x04,0x04,0x02,0x18,0x08, // ���q
        0x00,0xFF,0x2F,0x00 // �g���b�N�G���h
    };
    itoh(tmp1_1, (int)(0.5 * 1000000.0 * 120 / (double)bpm), 3); 
    std::copy(tmp1_1.begin(), tmp1_1.end(), std::back_inserter(track1));
    std::copy(tmp1_2.begin(), tmp1_2.end(), std::back_inserter(track1));


    // �ŏI�I�ȃf�[�^
    std::vector<UCHAR> fin = {};
    std::copy(header.begin(), header.end(), std::back_inserter(fin));
    std::copy(track1.begin(), track1.end(), std::back_inserter(fin));
	for(int i=0; i < (int)tracks.size(); ++i){
		std::copy(tracks.at(i).begin(), tracks.at(i).end(), std::back_inserter(fin));
	}


    // UCHAR�z��ɒ���
    UCHAR arr[10000];
    for (int i = 0; i < (int)fin.size(); ++i) {
        arr[i] = fin.at(i);
    }

    FILE* fp;
    fp = fopen("res.mid", "wb");
    if (fp == NULL) return -1;
    fwrite(arr, sizeof(UCHAR), fin.size(), fp);
    fclose(fp);
    
	return 0;
}