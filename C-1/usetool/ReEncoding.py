import os
import sys
import chardet

def convert(file_path, dst_code):
	encoding = detect(file_path)['encoding']
	if is_similar_encoding(encoding, dst_code):
		return encoding, 1

	with open(file_path, encoding=encoding) as fp:
		s = fp.read()
		
	with open(file_path, "w", encoding=dst_code) as fp:
		fp.write(s)
	return encoding, 0

def detect(file_path):
	with open(file_path, "rb") as fp:
		s = fp.read()
		return chardet.detect(s)

def is_similar_encoding(en1,en2):
	similar_group = [
		('ascii'),
		('gbk', 'GB2312'),
		('utf-8')
	]
	if en1 == 'ascii' or en2=='ascii':
		return True
	for group in similar_group:
		if en1 in group:
			if en2 in group:
				return True
			else:
				return False

if __name__ == '__main__':
	ext_list = ['.c', '.cpp', '.h']
	if len(sys.argv)<3:
		print("Usage: dir1 dir2 ... encoding")
		exit()
	
	dst_code = sys.argv[-1]
	if dst_code not in ["gbk", "utf-8"]:
		print("Only support gbk and utf-8")
		exit()
	
	cnt1 = 0
	cnt2 = 0
	for root in sys.argv[1:-1]:
		print("\nIn dir: %s\n"%root)

		for path, dir_list, file_list in os.walk(root):
			for file in file_list:
				_, ext = os.path.splitext(file)
				if ext in ext_list:
					full_file_name = os.path.join(path, file)

					encoding, flag = convert(os.path.join(path, file), dst_code)
					if flag:
						cnt1 += 1
						print("%2d %-8s %-30s don't change."%(cnt1+cnt2, encoding, full_file_name))
					else:
						cnt2 += 1
						print("%2d %-8s %-30s converted."%(cnt1+cnt2, encoding, full_file_name))
	print("\nInfo:\n changed %d file \n %d file don't change"%(cnt2, cnt1))