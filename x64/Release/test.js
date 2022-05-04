function test() {
    var a = 5;
    var b = 15;
    var c = 23.3;

    log("This is from JavaScript!!!\n");

    log("-----------\n")

    var e = new JSBindTest();
    log(e.age + "\n");

    e.age = 42;
    log(e.age + ", updated.\n");

    log("Object: " + e + "\n");

    return "Hello!" + (a + b + c);
}

test();