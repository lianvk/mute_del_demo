#include <iostream>
#include <cstdio>
#include <cmath>
#include <string>
#include <stdio.h>
#include <tchar.h>
#include <conio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <cstdlib>
#include <vector>
#include <set>
#include<algorithm>

using namespace std;

int s = 0;
long sum;
int a = 0; //процент от максимума 8 бит
int k = 0; //процент от максимума 16 бит
int g = 0;
int q = 0;
float arr;
float sum_value;
float value_sr;
vector<int> v;
vector<int> b;

unsigned long duration;

//Wav Header
struct wav_header_t
{
	char chunkID[4]; 
	uint32_t chunkSize; 
	char format[4]; 
	char subchunk1ID[4]; 
	unsigned long subchunk1Size; 
	uint16_t audioFormat;
	uint16_t numChannels;
	uint32_t sampleRate;
	uint32_t byteRate;
	uint16_t blockAlign;
	uint16_t bitsPerSample;
};

struct chunk_t
{
	char ID[4]; 
	uint32_t size;  
};

void WavReader(const char* fileName, const char* fileToSave)
{
	string filename;
	cout << "filename: ";
	getline(std::cin, filename);
	string filetype;
	cout << "filetype: ";
	getline(std::cin, filetype);
	string acmd0 = "ffmpeg -i ";
	string acmd1 = filename;
	string acmd2 = ".";
	string acmd3 = filetype;
	string acmd4 = " -acodec pcm_s16le -ac 2 audio.wav";
	//string acmd4 =" -vn -ar 44100 -ac 2 -ab 192K -f wav audio.wav";
	string acmd = acmd0 + acmd1 + acmd2 + acmd3 + acmd4;
	cout << acmd << "\n" << endl;
	system(acmd.c_str());
	FILE *fin = fopen(fileName, "rb");

	//Read WAV header
	wav_header_t header;
	fread(&header, sizeof(header), 1, fin);

	//Reading file
	chunk_t chunk;
	//printf("id\t" "size\n");
	
	while (true)
	{
		fread(&chunk, sizeof(chunk), 1, fin);
	//	printf("%c%c%c%c\t" "%li\n", chunk.ID[0], chunk.ID[1], chunk.ID[2], chunk.ID[3], chunk.size);
		if (*(unsigned int *)&chunk.ID == 0x61746164)
		{
			break;
		}
		fseek(fin, chunk.size, SEEK_CUR);
	}

	int duration = floor(chunk.size * 8 / header.bitsPerSample / (header.sampleRate*header.numChannels));

	int sample_size = header.bitsPerSample / 8;
	int samples_count = chunk.size * 8 / header.bitsPerSample;
	//printf("Samples count = %i\n", samples_count);
	printf("duration in sec: %d\n", duration);

	short int *value = new short int[samples_count];
	memset(value, 0, sizeof(short int) * samples_count);

	short int max = 0;
	short int delta = 0;
	
	
	for (int i = 0; i < samples_count; i++)
	{
		fread(&value[i], sample_size, 1, fin);
	}

	for (int i = 0; i < samples_count; i += header.sampleRate)
	{
		if (abs(value[i]) > abs(max))
		{
			max = value[i];
		//	printf("maximum : %d\n", max);
		}
		i = i + 2;
	}

	printf("maximum : %d\n", abs(max));


	//Write data into the file
	FILE *fout = fopen(fileToSave, "w");
	double j = 0;
	int deltaMax;
	cout << "Please, enter threshold: ";
	cin>>deltaMax;
	k = deltaMax;
	//fprintf(fout, "timing - \n");
	if (header.bitsPerSample == 16) {
		float t1 = 0;
		float t2 = 0;

		int g = 0;

		delta = abs(max) * k/100;
		
		printf("delta = %d\n\n", delta);
			//fprintf(fout, "Silence\n\n");
		
		for (int i = 0; i < samples_count; i += header.sampleRate)
			//for (int i = 0; i < samples_count; i += 2)
		{
			//if (i%header.sampleRate == 0) {
			//int c = (value16[i + 1] << 8) | value[i]; // WAV files are little-endian
			//double t = (c - 32768) / 32768.0;
			
			if ( abs(value[i]) < delta  && abs(value[i + 1])  < delta && (s == 0)) {
				
				//printf("Silence begin %f sec\n", j);
				s++;
				t1 = j;
				//fprintf(fout, "%f sec ", j);
				//fprintf(fout, "value = %d\n", value[i]);
				//fprintf(fout, "value = %d\n", value[i+1]);
		
			}
			if (!(abs(value[i]) < delta) | !(abs(value[i + 1])  < delta) && !(s == 0))
			{
				
				//printf("Silence end %f sec\n", j);
				
				//fprintf(fout, "- %f sec \n", j);

				//fprintf(fout, "%f sec \n", t);
				s = 0;
				t2 = j - t1;
				//fprintf(fout, "%f sec \n", t2);
				//fprintf(fout, "value = %d\n", value[i]);
				//fprintf(fout, "value = %d\n", value[i + 1]);
			}
			/*if ((abs(value[i]) < k) && (abs(value[i + 1])  < k) && !(s == 0))
			{
				//printf("Still silence");
				//continue;
				t += 0.5;

			}*/
			
			if ((abs(value[i]) < delta) | (abs(value[i + 1])  < delta) && (s == 1) && j == duration)
			{
				
				//printf("Silence end %f sec\n", j);
				
				//fprintf(fout, "- %f sec \n", j);
				t2 = j - t1;
				//fprintf(fout, "%f sec \n", t2);
				s = 0;
				//fprintf(fout, "value = %d\n", value[i]);
				//fprintf(fout, "value = %d\n", value[i + 1]);
				//q = 0;
				//t = 0;
			}

			
			//fprintf(fout, "%f sec: ", j);
			//fprintf(fout, " %d   ", value[i]);
			//fprintf(fout, " %d   \n\n", value[i + 1]);

			i = i + 2;
			j += 0.5;
			//if (j > duration) { break; }
			
			
			//	}
			if (t2 >= 3)
			{

				//arr = t1;
				v.push_back(t1);
				b.push_back(t2);
				/*string cmd1 = "(ffmpeg -i C:/rick.avi -ss ";
				string a = to_string(t1);
				string cmd2 = " -t ";
				string b = to_string(t2);
				string cmd3 = " ";
				string c = to_string(q);
				string cmd4 = ".avi)";
				string cmd = cmd1 + a + cmd2 + b + cmd3 + c + cmd4;
				
				cout << cmd << "\n" << endl;
				//cout << t1 << "\n" << endl;
				//cout << t2 << "\n" << endl;
				//system(cmd.c_str());*/
				q++;
				t1 = 0;
				t2 = 0;
				//cout << arr << " ";
			}
			
			
		}
		
		v.push_back(duration);
		
		cout << "q = " << q << endl;
		for (int n : v) {
			std::cout << "v=" << n << '\n';
			/*string cmd1 = "(ffmpeg -i C:/rick.avi -ss ";
			string a = to_string(b[n]);
			string cmd2 = " -t ";
			string b = to_string(v[n]);
			string cmd3 = " ";
			string c = to_string(q);
			string cmd4 = ".avi)";
			string cmd = cmd1 + a + cmd2 + b + cmd3 + c + cmd4;
			cout << cmd << "\n" << endl;*/
		}
		for (int n : b) {
			std::cout << "b=" << n << '\n';
		}
		if (v[0] == 0) {
			for (int j = 0; j < q; j++) {
				string bcmd0 = "ffmpeg -i ";
				string bcmd1 = filename;
				string bcmd2 = ".";
				string bcmd3 = filetype;
				string bcmd4 = " -ss ";
				string a = to_string(v[j] + b[j]);
				string bcmd5 = " -t ";
				string c = to_string(v[j + 1] - v[j] - b[j]);
				//string cmd7 = " -codec copy ";
				string bcmd6 = " ";
				string d = to_string(j);
				string bcmd7 = ".";
				string bcmd8 = filetype;
				string bcmd = bcmd0 + bcmd1 + bcmd2 + bcmd3 + bcmd4 + a + bcmd5 + c + bcmd6 + d + bcmd7 + bcmd8;
				cout << bcmd << "\n" << endl;
				system(bcmd.c_str());
				system("pause");
				fprintf(fout, "file '%d.%s'\n", j, filetype.c_str());
				//system("pause");
			}
	
		}
		if (!v[0] == 0)
		{
			v.insert(v.begin(), 0);
			b.insert(b.begin(), 0);
			for (int j = 0; j < q + 1; j++)
			{
				string ccmd0 = "ffmpeg -i ";
				string ccmd1 = filename;
				string ccmd2 = ".";
				string ccmd3 = filetype;
				string ccmd4 = " -ss ";
				string a = to_string(v[j] + b[j]);
				string ccmd5 = " -t ";
				string c = to_string(v[j + 1] - v[j] - b[j]);
				//string cmd7 = " -codec copy ";
				string ccmd6 = " ";
				string d = to_string(j);
				string ccmd7 = ".";
				string ccmd8 = filetype;
				string ccmd = ccmd0 + ccmd1 + ccmd2 + ccmd3 + ccmd4 + a + ccmd5 + c + ccmd6 + d + ccmd7 + ccmd8;
				cout << ccmd << "\n" << endl;
				system(ccmd.c_str());
				system("pause");
				fprintf(fout, "file '%d.%s'\n", j, filetype.c_str());
			}
		}
		q = 0;
	

	}
	if (header.bitsPerSample == 8)
	{
		delta = abs(max) * a / 100;
		fprintf(fout, "Silence\n\n");
		for (int i = 0; i < samples_count; i += header.sampleRate)
		//for (int i = 0; i < samples_count; i += 2)
		{
			//if (i%header.sampleRate == 0) {
				int k = 7;
				if (abs(abs(value[i]) - 128) < (k * 128) / 100 && abs(abs(value[i + 1]) - 128) < (k * 128) / 100 && (s == 0)) {
					//printf("Value %f \n", value[i]);
					printf("Silence begin %f sec\n", j);
					s++;
					
					fprintf(fout, "%f sec ", j);		
				}
				if (!(abs(abs(value[i]) - 128) < (k * 128) / 100) | !(abs(abs(value[i + 1]) - 128) < (k * 128) / 100) && !(s == 0))
				{
					//printf("Value %f \n", value[i]);
					printf("Silence end %f sec\n", j);
					//printf("%d\n", s);
					fprintf(fout, "- %f sec \n", j);
					s = 0;
				}
					
					if ((abs(abs(value[i]) - 128) < (k * 128) / 100) | (abs(abs(value[i + 1]) - 128) < (k * 128) / 100) && (s == 1) && j == duration)
				{
					printf("Silence end %f sec\n", j);
					
					fprintf(fout, "- %f sec \n", j);
					s = 0;
				}

				//fprintf(fout, "%f sec: ", j);
				//fprintf(fout, " %d   ", value[i]);
				//fprintf(fout, " %d   \n\n", value[i + 1]);

				i = i + 2;
				j += 0.5;
				//if (j > duration) { break; }
				
		//	}

		}
	}
		fclose(fin);
		fclose(fout);

		string dcmd0 = "ffmpeg -f concat -i list.txt -c copy output.";
		string dcmd1 = filetype;
		string dcmd = dcmd0 + dcmd1;
		cout << dcmd << "\n" << endl;
		system(dcmd.c_str());
	}

	


int main()
{
WavReader("audio.wav", "list.txt");
return 0;
}

