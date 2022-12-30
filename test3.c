int gcd(int u, int v)
{
	int a;
	int b;
	int c;
	int array[5];

	a = 5;
	b = 6;
	c = a/b;
	c = a*b/a+c-b;

	if(c>3) {
		array[3]=2;
	} else if(u<0){
		return u ;
	} else {
		return gcd(v,u-u/v*v);
	}
}

void main(void)
{
	int x; int y;
	x = input(); y = input();
	output(gcd(x,y));

	if (x>=5) {
	} else if(x>5) {
	} else if(x==5) {
	} else if(x<=5) {
	} else if(x<5) {
	} else if(x!=5) {
	}

	while(x<4){
		x=y*y*y*y*y;
		gcd(c[3]);
		gcd( gcd(a), gcd(b , gcd(aa,bb,cc,dd,ee)) ) ;
	}
}

