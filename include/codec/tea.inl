�򵥵�TEA�����㷨
TEA(Tiny Encrypt Algorithm)��һ�ּ�Ϊ�򵥵ĶԳƼ����㷨�����ñȽ��ձ飬������ͨ���㷨�ĸ���������֤�ģ������������ܵ���������֤�������㷨����һ��128λ����Կ������64λ���������ģ��ܲ���һ��64λ�����ġ����нϺõĿ�������ܡ�
�����㷨���£�
#define ROUNDS 32 /*��������*/
#define DELTA 0x9e3779b9 /* sqr(5)-1 * 2^31 */
#include "ctypes.h"
/**********************************************************
 Input values: k[4] 128λ��Կ
 v[2] ����ʱ64λ���ģ�����ʱ64λ����
 Output values: v[2] ����ʱ64λ���ģ�����ʱ64λ����
 **********************************************************/
void tea(word32* k, word32* v, long N) //���NΪ��ֵ���ǽ��ܹ��̣���Ӧ��v��Ϊ���ģ���Կkһ������k[0]��k[1]��k[2]��k[3]�ĸ�Ԫ��
{
  word32 y = v[0], z = v[1]; //yΪ���Ļ����ĸ�32λ��zΪ���Ļ����ĵ�32λ
  word32 limit, sum = 0; //sumΪ���ֺ�
  if (N > 0) { /* ���ܹ���*/
    limit = DELTA * N;
    while (sum != limit) { //ע�⣺��λ�͵�λ�������㣬����sum�����ĵ���λ������Կ�Ĳ���ѡ��
      y += ((z << 4) ^ (z >> 5)) + (z ^ sum) + k[sum & 3];
      sum += DELTA;
      z += ((y << 4) ^ (y >> 5)) + (y ^ sum) + k[(sum >> 11) & 3];
    }
  }
  else { /* ���ܹ��� �����Ǽ����㷨�򵥵ķ�������*/
    sum = DELTA * (-N);
    while (sum) {
      z -= ((y << 4) ^ (y >> 5)) + (y ^ sum) + k[(sum >> 11) & 3];
      sum -= DELTA;
      y -= ((z << 4) ^ (z >> 5)) + (z ^ sum) + k[sum & 3];
    }
