# versatus-c

# We use json-c from https://github.com/json-c/json-c
git clone  https://github.com/json-c/json-c

As per https://emscripten.org/docs/compiling/Building-Projects.html
Recompile json-c using emmake.

cd  ~/json-c-build-wasm
emconfigure ~/json-c/cmake-configure

cd ~/json-c
emmake make

emmake make install

You should see libjson-c.a in
~/emsdk/upstream/emscripten/cache/sysroot/lib/libjson-c.a

sudo cp ~/emsdk/upstream/emscripten/cache/sysroot/lib/libjson-c.a /usr/local/lib/libjson-c.a


# Compile smart contract

❯ cd contract-examples

❯ make
gcc -std=c99  -I/usr/local/include -o local main.c -L/usr/local/lib -ljson-c
emcc -std=c99  -I/usr/local/include -o contract.wasm main.c -L/usr/local/lib -ljson-c
wasm2wat contract.wasm  -o contract.wat
❯ 

# Run the local executable smart contract

❯ ./local < sample-contract-input.json
Version: 1
Account Address: contract_wallet_id
Account Balance: 65535
Protocol Version: 1
Block Height: 31415972
Block Time: 1694152781
Contract Function: splitEvenly
Amount: 100
Recipients:
- wallet_id_1
- wallet_id_2
- wallet_id_3
Recipient: wallet_id_1, Amount: 33
Recipient: wallet_id_2, Amount: 33
Recipient: wallet_id_3, Amount: 33

# Run the contract.wasm using wasmer

❯ wasmer contract.wasm \
< sample-contract-input.json

Version: 1
Account Address: contract_wallet_id
Account Balance: 65535
Protocol Version: 1
Block Height: 31415972
Block Time: 1694152781
Contract Function: splitEvenly
Amount: 100
Recipients:
- wallet_id_1
- wallet_id_2
- wallet_id_3
Recipient: wallet_id_1, Amount: 33
Recipient: wallet_id_2, Amount: 33
Recipient: wallet_id_3, Amount: 33

# Clean up
❯ make clean
rm -f local contract.wasm contract.wat
