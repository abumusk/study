#include "Exception.h"

const char* Exception::what() const throw ()
{
	return message_.c_str();
}

const char* Exception::StackTrace() const throw ()
{
	return stackTrace_.c_str();
}

void Exception::FillStackTrace() const throw ()
{
	//��Ӧ��linux ����ջ����
	//const int len = 200;
	//void* buffer[len];
	//int nptrs = ::backtrace(buffer, len);
	//char** strings = ::backtrace_symbols(buffer, nptrs);
	//if (strings)
	//{
	//	for (int i=0; i<nptrs; ++i)
	//	{
	//		stackTrace_.append(strings[i]);
	//		stackTrace_.push_back('\n');
	//	}
	//	free(strings);
	//}
}

//int backtrace(void **buffer,int size)
//�ú��������ȡ��ǰ�̵߳ĵ��ö�ջ,��ȡ����Ϣ���ᱻ�����buffer��,����һ��ָ�����顣���� size ����ָ��buffer�п��Ա�����ٸ�void* Ԫ�ء���������ֵ��ʵ�ʻ�ȡ��ָ�����,��󲻳���size��С��buffer�е�ָ��ʵ���ǴӶ�ջ�л�ȡ�ķ��ص�ַ,ÿһ����ջ�����һ�����ص�ַ��
//ע��ĳЩ���������Ż�ѡ��Ի�ȡ��ȷ�ĵ��ö�ջ�и���,������������û�ж�ջ���;ɾ�����ָ��Ҳ��ʹ�޷���ȷ������ջ����
//char ** backtrace_symbols (void *const *buffer, int size)
//backtrace_symbols����backtrace������ȡ����Ϣת��Ϊһ���ַ�������. ����bufferӦ���Ǵ�backtrace������ȡ������ָ��,size�Ǹ������е�Ԫ�ظ���(backtrace�ķ���ֵ)����������ֵ��һ��ָ���ַ��������ָ��,���Ĵ�Сͬbuffer��ͬ.ÿ���ַ���������һ�������buffer�ж�ӦԪ�صĿɴ�ӡ��Ϣ.��������������������ƫ�Ƶ�ַ,��ʵ�ʵķ��ص�ַ
//����,ֻ��ʹ��ELF�����Ƹ�ʽ�ĳ���Ϳ��Բ��ܻ�ȡ�������ƺ�ƫ�Ƶ�ַ.������ϵͳ,ֻ��16���Ƶķ��ص�ַ�ܱ���ȡ.����,�������Ҫ������Ӧ�ı�־��������,����֧�ֺ���������(����,��ʹ��GNU ld��ϵͳ��,����Ҫ����(-rdynamic))
//backtrace_symbols���ɵ��ַ�������malloc�����ģ����ǲ�Ҫ���һ��һ����free����Ϊbacktrace_symbols�Ǹ���backtrace������call stack������һ���Ե�malloc����һ���ڴ�����Ž���ַ����ģ����ԣ����������һ����ֻ��Ҫ�����free backtrace_symbols�ķ���ָ���OK�ˡ���һ��backtrace��manual��Ҳ���ر��ᵽ�ġ�
//ע��:�������Ϊ�ַ�����ȡ�㹻�Ŀռ亯���ķ���ֵ����ΪNULL
//   void backtrace_symbols_fd (void *const *buffer, int size, int fd)
//   backtrace_symbols_fd��backtrace_symbols ����������ͬ�Ĺ���,��ͬ����������������߷����ַ�������,���ǽ����д���ļ�������Ϊfd���ļ���,ÿ��������Ӧһ��.������Ҫ����malloc����,����������п��ܵ��øú�����ʧ�ܵ������