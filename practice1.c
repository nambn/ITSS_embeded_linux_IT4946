int foo (int i);
void var (int val);

int main ()
{
	foo (5);
	return 0;
}

int foo (int i)
{
	var(i);
	return (0);
}

void var(int val)
{
	int i = 0;
	for ( ; val>0; val--){
			i += val;
	}
}
