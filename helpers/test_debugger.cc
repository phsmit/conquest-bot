class TestDebugger {
public:
  int a;

  TestDebugger(): a(3) {}

  int get_a() {
    return a;
  }
};

int main(int argc, const char *argv[]) {

  TestDebugger t;
  t.get_a();

  return 0;
}