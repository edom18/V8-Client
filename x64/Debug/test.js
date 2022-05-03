function Hoge(name) {
    this.name = name;
}

function test() {
    var a = 5;
    var b = 15;
    var c = 23.3;

    var hoge = new Hoge("edo");

    log("fuga");

    return "Hello!" + (a + b + c + 8.19) + hoge.name + "<<<";
}

test()